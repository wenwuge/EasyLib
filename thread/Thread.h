#ifndef __THREAD
#define __THREAD
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <stdint.h>
#include <tr1/functional>
#include <tr1/memory>
#include "thread_local_storage.h"

using namespace std;
#define H_LINUX_THREAD_PRIORITY
class Thread{
    public:
        typedef pthread_t ThreadHandle;
        typedef pthread_t TID;
        enum ThreadPriority
        {

            THREAD_PRIORITY_MAX = 0,  /**< The maximum possible priority  */
            THREAD_PRIORITY_HIGH,     /**< A high (but not max) setting   */
            THREAD_PRIORITY_NORMAL,  /**< An average priority            */
            THREAD_PRIORITY_LOW,      /**< A low (but not min) setting    */
            THREAD_PRIORITY_MIN,      /**< The minimum possible priority */
            THREAD_PRIORITY_DEFAULT   /**< Priority scheduling default    */

        };
        

        //! Thread state.
        enum ThreadState
        {
            S_NULL = 0,  //! Thread is not initialized.
            S_STARTING,  //! Thread is starting, but has not entered run(...) method.
            S_RUNNING,   //! Thread is running in run(...) method.
            S_STOPPING,  //! Thread is required stopping.

            S_FORCECHAR = 0xff,
        };
    
    public:
        Thread();

        ~Thread();

        //! Sets thread running priority.
        //! \remark It can ONLY be called before call start.
        void setPriority( ThreadPriority nPriority );

        //! Gets current thread priority.
        ThreadPriority getPriority()const
        {
            return m_nPriority;
        }

        //! Sets thread processor affinity. e.g. "00001" us first processor.
        void setThreadAffinity( uint32_t nMask );


        //! Start the thread.
        //! \remark It can ONLY start thread when thread is stopped or never started.
        //!         Also, the method can be overwrote by derived class to ease work on check any extended states.
        //! \return false if can not start. You can only start a thread when state is S_NULL,
        virtual bool start();

        //! Stop the thread.
        //! \remark It only send a exit flag and it is up to derived thread object to response the flag.
        //!         Also, the method can be overwrote by derived class to ease work on check any extended states.
        //! it will not blocked.
        virtual void stop();

        //! Join the work thread. It will wait for exit of the thread.
        //! \param msWait How long to wait before the thread is exited.
        //! \remark It is called by main thread to stop the thread. It will block, so derived class MUST
        //! response S_STOPPING flag.
        //! \return false if can not wait the thread to exit and wait time out.
        virtual bool join( uint32_t msWait = uint32_t( -1 ) );

        //! Terminate the working thread.
        //! \reamrk Main thread can call the method to force to terminate the thread.
        //! \warning It is quite dangerous!!!
        virtual void terminate();


        //! Query whether the thread is running.
        bool isRunning() const
        {
            return m_nState == S_RUNNING;
        }

        //! Query whether the thread has been stopped.
        bool isStopped() const
        {
            return m_nState == S_NULL;
        }

        //! Get the thread's state
        ThreadState getState() const 
        {
            return m_nState;
        }

        //! Sets name for the thread.
        void setName( const string& strName )
        {
            m_strName = strName;
        }

        //! Gets name of the thread.
        const string& getName()const
        {
            return m_strName;
        }

        //! Gets handle.
        ThreadHandle getThreadHandle()const
        {
            return m_hThread;
        }

        TID tid() const 
        {
            return m_hThread;
        }

        //! Gets current thread. Get it from thread local storage.
        static Thread* getCurrentThread();


        //! Gets working thread number
        //! \note Main thread is NOT counted.
        static int32_t getNumWorkingThread();


        //! Get the current thread id
        static ThreadHandle getCurrentThreadID();
        //static ThreadHandle gettid() { return getCurrentThreadID();}
        static const string& getTidString();


    protected:

        //! Thread function.
        //! \note Derived class should check S_STOPPING flag and to give up thread.
        virtual void run() = 0;


    private:
        friend   class        MainThread;   //! Main thread
        volatile ThreadState    m_nState;   //! object state. It is maintained by thread.
        ThreadPriority       m_nPriority;

        string                 m_strName;   //! Thread Name;

        //ThreadPolicy           m_nPolicy;
        ThreadHandle           m_hThread;

        pthread_attr_t     m_thread_attr;

    private:
        static void* ThreadProc( void* pvArg );
        uint32_t getProcessAffinity();

    private:
        // Do the thread work.
        void doWork();
};

class  ThreadRunner : public Thread
{
public:
    typedef std::tr1::function< void () > Handler;
    ThreadRunner(Handler handler) 
        : handler_(handler)
    {
    }

protected:
    void setHandler(Handler handler) { handler_ = handler; }

private:
    virtual void run()
    {
        if (handler_)
        {
            handler_();
        }
    }

private:
    Handler handler_;
};
#endif
