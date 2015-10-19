#include "Thread.h"
#include "../common/atomic_op.h"
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
pthread_key_t   s_thread_key = 0;

volatile int32_t  s_nNumWorkingThread = 0;

volatile int32_t  s_nNumThreads = 0;
Thread* s_pMainThread = NULL;
static ThreadLocal<Thread::ThreadHandle> t_tid;
static ThreadLocal<string>              t_tidstring;//for Logging
class MainThread : public Thread
{
public:
    MainThread()
    {
        s_pMainThread = this;

        //m_hThread
        pthread_key_create( &s_thread_key, NULL );
        pthread_setspecific( s_thread_key, this );
        m_nState = S_RUNNING;
    }

    ~MainThread()
    {
        m_nState = S_NULL;

        s_pMainThread = NULL;
        pthread_key_delete(s_thread_key);
        s_thread_key = 0;
    }

    bool start()
    {
        return true;
    }
    void stop()             {}
    bool join( uint32_t msWait )
    {
        (void)msWait;
        return false;
    }
    virtual void terminate() {}
    void run()
    {
        // do nothing.
    }
};

//----------------------------------------------
Thread::Thread()
    : m_nState( Thread::S_NULL )
    , m_nPriority( Thread::THREAD_PRIORITY_NORMAL )
    , m_hThread( 0 )
{
    if ( InterlockedInc32( &s_nNumThreads ) == 1 && !s_pMainThread )
    {
        new MainThread();
    }

    // set initial name.
    m_strName = "Unknown Thread";

    pthread_attr_init( &m_thread_attr );
    pthread_attr_setschedpolicy( &m_thread_attr, SCHED_RR );
    pthread_attr_setdetachstate (&m_thread_attr, PTHREAD_CREATE_DETACHED);
}

//----------------------------------------------
Thread::~Thread()
{
    // thread.
    if ( InterlockedDec32( &s_nNumThreads ) == 1 && s_pMainThread )
    {
        delete s_pMainThread;
    }

    if ( m_nState != S_NULL )
    {
        terminate();
    }

    pthread_attr_destroy( &m_thread_attr );
}

//----------------------------------------------
Thread* Thread::getCurrentThread()
{
    return static_cast<Thread *>( pthread_getspecific( s_thread_key ) );

}

//----------------------------------------------
int32_t Thread::getNumWorkingThread()
{
    return s_nNumWorkingThread;
}

//----------------------------------------------
void Thread::setPriority( Thread::ThreadPriority nPriority )
{
    m_nPriority = nPriority;

    if ( !m_hThread )
    {
        return;
    }

    struct sched_param param;
    switch ( m_nPriority )
    {
    case THREAD_PRIORITY_MAX:
        param.sched_priority = 99;
        break;
    case THREAD_PRIORITY_HIGH:
        param.sched_priority = 80;
        break;
    case THREAD_PRIORITY_NORMAL:
        param.sched_priority = 70;
        break;
    case THREAD_PRIORITY_LOW:
        param.sched_priority = 60;
        break;
    case THREAD_PRIORITY_MIN:
        param.sched_priority = 50;
        break;
    case THREAD_PRIORITY_DEFAULT:
    default:
        param.sched_priority = 70;
        break;
    }
    pthread_attr_setschedparam( &m_thread_attr, &param );
}
//----------------------------------------------
uint32_t  Thread::getProcessAffinity()
{
    cpu_set_t st;
    CPU_ZERO( &st );

    uint32_t nProcessorMask = 0;

    if( sched_getaffinity( 0, sizeof( cpu_set_t ), &st ) == 0 )
    {
     for( uint32_t i = 0; i < 32; ++i )
     {
         if( CPU_ISSET( i, &st ) )
         {
             nProcessorMask |= ( 1 << i );
         }
     }
    }

    return nProcessorMask;
}

void Thread::setThreadAffinity( uint32_t nMask )
{
    uint32_t nProcessMask = getProcessAffinity();

    if( ( nMask & nProcessMask ) == 0 )
    {
        nMask = nProcessMask;
    }

    cpu_set_t st; 
    CPU_ZERO( &st );

    bool bValid = false;

    for( uint32_t i = 0; i < 32; ++i )
    {   
        if( ( nProcessMask & ( 1 << i ) ) != 0 ) 
        {   
            CPU_SET( i, &st );

            bValid = true;
        }   
    }   

    if( !bValid )
    {   
        CPU_SET( 0, &st );
    }
    ::sched_setaffinity( 0, sizeof( cpu_set_t ), &st );


}
//----------------------------------------------
bool Thread::start()
{
    if ( m_nState == S_NULL )
    {
        // sett state.
        m_nState = S_STARTING;

        // assure thread is null.
        m_hThread = 0;

        // create thread and start it.

        setPriority( m_nPriority );
#ifdef H_LINUX_THREAD_PRIORITY
        int ret = pthread_create( &m_hThread, &m_thread_attr, &Thread::ThreadProc, this );
#else
        int ret = pthread_create( &m_hThread, NULL, &Thread::ThreadProc, this );
#endif
        if ( 0 == ret ) // create successfully
        {
            return true;
        }
        // reset state.
        m_nState = S_NULL;
    }

    return false;
}
//----------------------------------------------
void Thread::stop()
{
    if ( m_nState != S_NULL )
    {
        m_nState = S_STOPPING;
    }
}
//----------------------------------------------
bool Thread::join( uint32_t msWait )
{
    if ( !m_hThread )
    {
        return true;
    }

    // stop first,
    stop();


    uint32_t msSpan = 5;

    while ( m_nState != S_NULL )
    {
        usleep( msSpan*1000 );

        if ( msWait < msSpan )
        {
            break;
        }

        msWait -= msSpan;
    }

    if ( m_nState == S_NULL )
    {
        assert( m_hThread == 0 );
        return true;
    }


    return true;
}
//----------------------------------------------
void Thread::terminate()
{
    if ( m_hThread )
    {
        //TODO
        pthread_kill( m_hThread, 9 );
        m_hThread = 0;

        m_nState = S_NULL;
    }
}

void* Thread::ThreadProc( void* pvArg )
{
    // maybe is stopping
    Thread* pThis = ( Thread* )pvArg;

    // set thread local staroge.
    pthread_setspecific( s_thread_key, pvArg );

    pThis->doWork();

    pthread_detach(pthread_self());

    return 0;
}


//----------------------------------------------
void Thread::doWork()
{
    
    if ( m_nState != S_STARTING )
    {
        m_nState = S_NULL;

        m_hThread = 0;
        return;
    }


    // set running flag.
    m_nState = S_RUNNING;

    // call run method.
    run();
    // set state.
    m_nState = S_NULL;

    // clear thread flag.
    m_hThread = 0;
}

Thread::ThreadHandle Thread::getCurrentThreadID()
{
    getTidString();
    return t_tid.value();
}

const string& Thread::getTidString()
{
    if (t_tidstring.value().empty())
    {
        Thread::ThreadHandle ret_tid;
        ret_tid = ::pthread_self();
        t_tid.value() = ret_tid;
        char buf[32] = {0};
        snprintf(buf, sizeof(buf),   "%ll", (uint64_t)ret_tid);
        t_tidstring.value() = buf;
    }

    return t_tidstring;
}

