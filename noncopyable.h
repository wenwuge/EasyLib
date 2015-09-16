#ifndef OSLIB_NONCOPYABLE_H_
#define OSLIB_NONCOPYABLE_H_

namespace osl
{
	class Noncopyable 
	{
    protected:
        Noncopyable () {};
        ~Noncopyable () {};
	private:
		Noncopyable ( const Noncopyable& );
		Noncopyable &operator=( const Noncopyable& );
	};
    typedef Noncopyable noncopyable;

    class copyable
    {
    };
}


