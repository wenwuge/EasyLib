#ifndef OSLIB_LOCKER_H__
#define OSLIB_LOCKER_H__



#include <unistd.h>
#include "../common/atomic_op.h"

#if ( defined(H_STAT_LOCK) || defined(H_STAT_MINI_LOCK) || defined(H_STAT_SPIN_LOCK) )
//#	include "HLogManager.h"
#endif

    typedef unsigned int u32;
    typedef int s32;
    static  int  s_nAnyCounter;

    //! The struct simulates a system lock atom.
    //! \remark It can only be used when chance of co-current conflict is very small.
    //! It use
    template<typename Locker>
    struct ScopedLock
    {
        Locker&   m_Lock;

        //! Constructor for the lock.
        //! \remark It will wait until a lock is free.
        //! \param bLock Lock object. It MUST be modified by volatile.
        ScopedLock( Locker& lk ) : m_Lock( lk )
        {
            m_Lock.lock();
        }

        ScopedLock( Locker* plk ) : m_Lock( *plk )
        {
            m_Lock.lock();
        }

        ~ScopedLock()
        {
            m_Lock.unlock();
        }
    private:
        ScopedLock( const ScopedLock& );
        ScopedLock&operator=( const ScopedLock& );
    };

    //! The struct simulates a system lock atom.
    //! \remark It can only be used when chance of co-current conflict is very small.
    //! It use
    template<typename Locker>
    struct ScopedTryLock
    {
        Locker&      m_Lock;
        bool      m_bLocked;
        //! Constructor for the lock.
        //! \remark It will wait until a lock is free.
        //! \param bLock Lock object. It MUST be modified by volatile.
        ScopedTryLock( Locker& lk ) : m_Lock( lk )
        {
            m_bLocked = m_Lock.tryLock();
        }

        ~ScopedTryLock()
        {
            if ( m_bLocked )
            {
                m_Lock.unlock();
            }
        }
    };
    //! A lightweight lock which use user level code.
    //! \warning The lock is not quit safe and can ONLY be used
    //! for cases where actually there is few conflicts may happen.
    struct  SpinLock
    {
        typedef ScopedLock<SpinLock>  MyScopedLock;


        SpinLock() 
        {
#ifdef H_STAT_SPIN_LOCK
            nNumConflicts = 0;
#endif
        };

#ifdef H_STAT_SPIN_LOCK
        u32    nNumConflicts;
#endif

        //
        bool needWait()
        {
           return ( m_bLocked != 0 || m_nNumWaitThread != m_nNumEnterThread );
        }

        void lock()
        {
            // if locked, should enter wait mode.
            if ( InterlockedInc32( &m_nNumEnterThread ) > 1 )
            {
                // wait lock for the lock
                for(;;)
                {
                    // add wait count.
                    InterlockedInc32( &m_nNumWaitThread );

                    // get wait lock
                    while ( needWait() )
                    {
#ifdef H_STAT_MINI_LOCK
                        ++nNumConflicts;
                        OStringStreamA strTip;
                        strTip << "MiniLock conflicts:" << nNumConflicts;
                        H_LOG_NAME_WARN( "", strTip.str() );
#endif
                        // spin for a while.
                        int nNumSpin = 2000;
                        int i = 0;

                        while ( i < nNumSpin && needWait() )
                        {
                            ++i;
                            ++s_nAnyCounter;
                        }

                        // if can not wait to enter,just sleep for a while
                        if( i >= nNumSpin )
                        {
                            sleep( 0 );
                        }
                    }

                    // 维持计数平衡
                    InterlockedDec32( &m_nNumWaitThread );

                    // 先关锁
                    if ( InterlockedInc32( &m_bLocked ) > 1 )
                    {
                        InterlockedDec32( &m_bLocked );
                        continue;
                    }
                    
                     // 有一个人获得了锁，逃离等待，进入。。
                    return;
                }
            }

            // lock it.
            InterlockedInc32( &m_bLocked );

        }

        //! Try lock
        bool tryLock()
        {
			// lock it.
			if( InterlockedInc32( &m_bLocked ) > 1 )
			{
				InterlockedDec32( &m_bLocked );
				return false;
			}

			/// Test lock for m_bLock.
			if ( InterlockedInc32( &m_nNumEnterThread ) > 1 )
			{
				InterlockedDec32( &m_nNumEnterThread );
				InterlockedDec32( &m_bLocked );
				return false;
			}
			return true;
        }

        void unlock()
        {
            InterlockedDec32( &m_nNumEnterThread );

            // lock it.
            InterlockedDec32( &m_bLocked );
        }
    private:
        volatile s32            m_nNumEnterThread;   //! Number of thread enter the lock
        volatile s32            m_nNumWaitThread;   //! Number of thread wait the lock.
        volatile s32                   m_bLocked;   //! whether is locked.
    };




    //! Critical section lock.
    struct   Lock
    {
        typedef ScopedLock<Lock>         MyScopedLock;
        typedef ScopedTryLock<Lock>   MyScopedTryLock;

#ifdef H_STAT_LOCK
        u32    nNumConflicts;
#endif


        Lock( u32 nSpinCount = 4000 )
            : m_nLockCount( 0 )
        {
#ifdef H_STAT_LOCK
            nNumConflicts = 0;
#endif
            (void)nSpinCount;//avoid gcc compile warning

            pthread_mutex_init( &m_Mutex, NULL );

        };
        ~Lock()
        {
            pthread_mutex_destroy( &m_Mutex );
        };

        void lock()
        {
#ifdef H_STAT_LOCK

            if ( m_nLockCount > 0 )
            {
                ++nNumConflicts;
                OStringStreamA strTip;
                strTip << "Lock conflicts:" << nNumConflicts;
                H_LOG_NAME_WARN( "", strTip.str() );
            }

#endif


            pthread_mutex_lock( &m_Mutex );
            ++m_nLockCount;
        }

        void unlock()
        {
            --m_nLockCount;

            pthread_mutex_unlock ( &m_Mutex );
        }

        //! try lock. It will not block anyway.
        //! \return false if not get the lock.
        bool tryLock()
        {

            if ( 0 == pthread_mutex_trylock( &m_Mutex ) )
            {
                ++m_nLockCount;
                return true;
            }

            return false;
        }

        bool isLocked()
        {
            bool bLocked = tryLock();

            if ( bLocked )
            {
                unlock();
            }

            return bLocked;
        }

        pthread_mutex_t          m_Mutex;
        volatile s32        m_nLockCount;  //! number of blocked on the lock.
    };
    typedef Lock MutexLock;
    typedef MutexLock::MyScopedLock MutexLockGuard;



    //! a null lock which do no lock at all.
    struct  NullLock
    {
        typedef ScopedLock<NullLock>  MyScopedLock;

        NullLock()
        {
        };
        void lock()
        {
        }
        //! Try lock
        bool tryLock()
        {
            return true;
        }

        void unlock()
        {
        }
    };



    //! The macro define a auto-lock guard.
#define H_MINI_AUTOLOCK( lockParam )     volatile osl::MiniLock::MyScopedLock malk_##lockParam(lockParam);
#define H_MINI_AUTOTRYLOCK( lockParam )  volatile osl::MiniLock::MyScopedTryLock malk_##lockParam(lockParam);

    //! The macro define a auto-lock guard.
#define H_SPIN_AUTOLOCK( lockParam )     volatile osl::SpinLock::MyScopedLock malk_##lockParam(lockParam);
#define H_SPIN_AUTOTRYLOCK( lockParam )  volatile osl::SpinLock::MyScopedTryLock malk_##lockParam(lockParam);

    //! The macro define a auto-lock guard.
#define H_AUTOLOCK( lockParam )     volatile osl::Lock::MyScopedLock malk_##lockParam(lockParam);
#define H_AUTOTRYLOCK( lockParam )  volatile osl::Lock::MyScopedTryLock malk_##lockParam(lockParam);


#endif // end of file.
