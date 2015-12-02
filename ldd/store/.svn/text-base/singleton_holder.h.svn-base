
#ifndef __LDD__SINGLETON_HOLDER_H_
#define __LDD__SINGLETON_HOLDER_H_




namespace ldd{
namespace storage{

template <typename T>
class SingletonHolder
{
private:
	SingletonHolder();
	SingletonHolder(const SingletonHolder&);
	SingletonHolder& operator=(SingletonHolder&);
public:
	static T& Instance()
	{
		static T instance_;
		return instance_;
	}

	static void Destory(){
		Instance().Destory();
	}
private:
};




}//namespace storage
}//namespace ldd


#endif //__LDD__SINGLETON_HOLDER_H_
