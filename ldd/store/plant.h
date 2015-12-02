#ifndef __LDD__STORAGE_PLANT_H_
#define __LDD__STORAGE_PLANT_H_


#include <ldd/util/mutex.h>
#include <ldd/util/atomic.h>
#include "thread_manager.h"





namespace ldd{
namespace storage{



class RequestProcessPlant : public  Runnable
{
public:
	RequestProcessPlant();
	virtual	~RequestProcessPlant();
	virtual void run();

public:
	virtual void schedule() = 0;
public:
	void Stop();

private:
	ldd::util::Atomic<bool>			stop_;
};

class ReadRequestProcessPlant : public RequestProcessPlant
{
public:
	ReadRequestProcessPlant(){};
	virtual ~ReadRequestProcessPlant();
	virtual void schedule();
private:
};

class WriteRequestProcessPlant : public RequestProcessPlant
{
public:
	WriteRequestProcessPlant(){};
	virtual ~WriteRequestProcessPlant();
	virtual void schedule();
private:
};


}//namespace storage
}//namespace ldd







#endif //__LDD__STORAGE_PLANT_H_
