#ifndef __REFPTR_H
#define __REFPTR_H
#include "atomic_op.h"
#include <iostream>
typedef int s32;

class RefTarget
{
    public:
        // Constructor & Destructor.
        RefTarget() {};
        virtual ~RefTarget() {};

        /** Increments the reference count and returns the new value.*/
        virtual void ref() const = 0;

        /** Decrements the reference count and returns the new value.*/
        virtual void unref() const = 0;

        /** Gets the reference count.*/
        virtual s32 getRef() const = 0;
};


//Object provide refer count for derived class
class Object : public RefTarget
{

public:
    Object(): m_nRefCount( 0 ) {}

    // need to override from derived class.
    virtual ~Object() {}


    virtual void ref()const
    {
        InterlockedInc32( &m_nRefCount );
    }

    virtual void unref() const
    {
        if ( InterlockedDec32( &m_nRefCount ) <= 0 )
        {
            delete this;
        }
    }
    
    s32 getRef() const
    {
        return ( s32 )m_nRefCount;
    }

    s32 use_count() const
    {
        return getRef();
    }

protected:
	mutable volatile s32    m_nRefCount;
};

//RefPtr manage class T, T derive the object having ref count
template<class T> class RefPtr
{
public:
    typedef T object_type;
protected:
    T* pRep;
public:
    RefPtr(): pRep( 0 ) {}

    RefPtr( object_type* rep )
    {
        pRep = rep;
        if ( pRep )
        {
            pRep->ref();
        }
    }

    // Get Pointer from other type
    template< class U >
    RefPtr( U* rep )
    {
        pRep = dynamic_cast<object_type*>( rep );

        if ( pRep )
        {
            pRep->ref();
        }
    }

    RefPtr( const RefPtr& r )
    {
        pRep = r.getPointer();

        if ( pRep )
        {
            pRep->ref();
        }
    }

    template< class U >
    RefPtr( const RefPtr<U>& r )
    {
        pRep = dynamic_cast<object_type*>( r.getPointer() );

        if ( pRep )
        {
            pRep->ref();
        }
    }

    // assigner
    RefPtr& operator=( const RefPtr& r )
    {

        bind( r.getPointer() );

        return *this;
    }

    // assigner
    RefPtr& operator=( object_type* rep )
    {

        bind( rep );

        return *this;
    }

    // assigner
    template< class U >
    RefPtr& operator=( const RefPtr<U>& r )
    {

        bind( dynamic_cast<object_type*>( r.getPointer() ) );

        return *this;
    }

    ~RefPtr()
    {
        setNull();
    }

    inline operator object_type*()const
    {
        return pRep;
    }
    inline object_type& operator*() const
    {
        assert( pRep );
        return *pRep;
    }
    inline object_type* operator->() const
    {
        assert( pRep );
        return pRep;
    }

    inline object_type* get() const
    {
        return pRep;
    }
    inline object_type* getPointer() const
    {
        return pRep;
    }
    inline bool isNull( void ) const
    {
        return pRep == 0;
    }
    /** Binds rep to the RefPtr.*/
    void bind( object_type* rep )
    {
        // MUST before setNull to assure object is relatively deleted.
        if ( rep )
        {
            rep->ref();
        }

        setNull();

        // copy data.
        pRep = rep;
    }

    void reset(object_type* rep = NULL)
    {
        bind(rep);
    }

    /** Clear reference */
    void setNull( void )
    {
        if ( pRep )
        {
            T* pThis = pRep;
            pRep = NULL;
            pThis->unref();
        }
    }

    void swap( RefPtr<T>& r )
    {
        std::swap( this->pRep , r.pRep );
    }
};
#endif
