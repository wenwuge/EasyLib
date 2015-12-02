

#ifndef __LDD__STORAGE_THREAD_MANAGER_H_
#define __LDD__STORAGE_THREAD_MANAGER_H_

#include <pthread.h>
#include <cassert>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <ldd/util/mutex.h>
#include <ldd/util/atomic.h>

using namespace std;

namespace ldd{
namespace storage{

/* The base class of the job for Thread Manager
* the run function needed to be overloaded
* */
class Runnable {
	public:
		Runnable();
		virtual ~Runnable();
		virtual void run();
};

class ThreadManager{
	/*
	 * create thread_num threads, append Job class's descent to the queue
	 * the thread will fetch job and run.
	 * */
public: 
	ThreadManager (unsigned int size);
	~ThreadManager (); 

	unsigned int size();        /* the job size in Thread Manager      */
	void append(Runnable *); /* append a job to the queue          */
	void sync();        /* wait till all the jobs are done    */
	void worker();      /* the slave process to fetch and run */

private:
	ldd::util::Mutex	mutex0_, mutex1_;
	ldd::util::CondVar  cond_;
	ldd::util::Atomic<bool> running_;
	unsigned int	thread_num_;
	unsigned int	running_jobs_;
	queue<Runnable *> jobs_;
	pthread_t *		threads_; 
};


}//namespace storage
}//namespace ldd

#if 0
class Task1 :public Runnable{
public:
	void run(void)
	{
		mut.lock();
		for (int i = 0;i < 100;i++) {
			usleep(1);
			printf("1");
			fflush(stdout);
		}
		putchar(10); fflush(stdout);
		mut.unlock();
	}
};

class Task2: public Task1
{
public: 
	void run()
	{
		mut.lock();
		for (int i = 0;i < 100;i++) {
			usleep(1);
			printf("2");
			fflush(stdout);
		}
		putchar(10); fflush(stdout);
		mut.unlock();
	}
};


class Task3 :public Job{
public:
	void run(void)
	{
		for (int i = 0;i < 100;i++) {
			usleep(1);
			printf("1");
			fflush(stdout);
		}
		putchar(10); fflush(stdout);
	}
};

class Task4: public Task1
{
public: 
	void run()
	{
		for (int i = 0;i < 100;i++) {
			usleep(1);
			printf("2");
			fflush(stdout);
		}
		putchar(10); fflush(stdout);
	}
};
int main(void)
{
	ThreadManager tm1(2);
	Task1 a;
	Task2 b;
	Task3 c;
	Task4 d;
	tm1.append(&a);
	tm1.append(&b);
	putchar(10);
	tm1.sync();
	tm1.append(&c);
	tm1.append(&d);
	return 0;
}
#endif



#endif //__LDD__STORAGE_THREAD_MANAGER_H_
