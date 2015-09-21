#ifndef OSLIB_ATOMIC_OPERATION_H_
#define OSLIB_ATOMIC_OPERATION_H_
typedef int s32;
#		define InterlockedCmpXchng32(ptr,n,o)     __cmpxchg32(ptr,o,n)
#		define InterlockedXchng32(ptr,v)          __xchg32(ptr,v)
#		define InterlockedInc32(ptr)              __xadd32(ptr,(s32)(1))
#		define InterlockedDec32(ptr)              __xadd32(ptr,-1)
#		define InterlockedExchangeAdd32( x, y )   __xadd32( (volatile s32*)x, (s32)y )

    s32 __cmpxchg32( volatile s32* ptr, s32 old, s32 n );
    s32 __xchg32( volatile s32* ptr, s32 x );
    s32 __xadd32( volatile s32* ptr, s32 i );

#ifdef H_ARCH_64
#		define InterlockedCmpXchng64(ptr,n,o)     __cmpxchg64( (volatile long*)ptr,o,n)
#		define InterlockedXchng64(ptr,v)          __xchg64( (volatile long*)ptr,(long)v )
#		define InterlockedInc64(ptr)              __xadd64( (volatile long*)ptr, (long)(1) )
#		define InterlockedDec64(ptr)              __xadd64( (volatile long*)ptr,(long)-1 )
#		define InterlockedExchangeAdd64( x, y )   __xadd64( (volatile long*)x, (long)y )
    long __cmpxchg64( volatile long* ptr, long old, long n );
    long __xchg64( volatile long* ptr, long x );
    long __xadd64( volatile long* ptr, long i );
#endif


#ifdef H_ARCH_32
#		define InterlockedCmpXchng                InterlockedCmpXchng32
#		define InterlockedXchng	                  InterlockedXchng32
#		define InterlockedInc		              InterlockedInc32
#		define InterlockedDec		              InterlockedDec32
#		define InterlockedXAdd( x, y )            InterlockedExchangeAdd32( x, y )

#else
#		define InterlockedCmpXchng                InterlockedCmpXchng64
#		define InterlockedXchng	                  InterlockedXchng64
#		define InterlockedInc		              InterlockedInc64
#		define InterlockedDec		              InterlockedDec64
#		define InterlockedXAdd( x, y )            InterlockedExchangeAdd64( x, y )

#endif
    //! Memory barrier.
#	define mb()  __sync_synchronize()

#include <stdint.h>
#ifdef H_ARCH_64
    inline long __xchg64( volatile long* ptr, long x )
    {
        __asm__ __volatile__( "xchgq %0,%1"
            : "=r" ( x )
            : "m" ( *ptr ), "0" ( x )
            : "memory" );

        return x;
    }
#endif
    inline int32_t __xchg32( volatile int32_t* ptr, int32_t x )
    {
        __asm__ __volatile__( "xchgl %k0,%1"
            : "=r" ( x )
            : "m" ( *ptr ), "0" ( x )
            : "memory" );
        return x;
    }

    /*
    * Atomic compare and exchange.  Compare OLD with MEM, if identical,
    * store NEW in MEM.  Return the initial value in MEM.  Success is
    * indicated by comparing RETURN with OLD.
    */
#define LOCK_PREFIX "lock ; "
#ifdef H_ARCH_64
    inline long __cmpxchg64( volatile long* ptr, long old, long n )
    {
        long prev;
        __asm__ __volatile__( LOCK_PREFIX "cmpxchgq %1,%2"
            : "=a"( prev )
            : "r"( n ), "m"( *ptr ), "0"( old )
            : "memory" );

        return prev;
    }
#endif
    inline int32_t __cmpxchg32( volatile int32_t* ptr, int32_t old, int32_t n )
    {
        long prev;
        __asm__ __volatile__( LOCK_PREFIX "cmpxchgl %k1,%2"
            : "=a"( prev )
            : "r"( n ), "m"( *ptr ), "0"( old )
            : "memory" );
        return prev;
    }
#ifdef H_ARCH_64
    inline long __xadd64( volatile long* ptr, long i )
    {
        long __i = i;
        __asm__ __volatile__( LOCK_PREFIX "xaddq %0, %1"
            : "+r" ( i ), "+m" ( *ptr )
            : : "memory" );

        return i + __i;
    }
#endif
    inline int32_t __xadd32( volatile int32_t* ptr, int32_t i )
    {
        long __i = i;
        __asm__ __volatile__( LOCK_PREFIX "xaddl %0, %1"
            : "+r" ( i ), "+m" ( *ptr )
            : : "memory" );
        return i + __i;
    }

#endif
