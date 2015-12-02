
#ifndef __LDD__JOB_H_
#define __LDD__JOB_H_

#include <stdint.h>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "ldd/util/slice.h"
#include "ldd/protocol/client/response.h"
#include "session.h"



namespace ldd{
namespace storage{


class Task;




class Job{
public:
	Job(boost::shared_ptr<Session>msg);
	virtual ~Job();
	void AddTask(Task* task, bool count_task);
    void RescheduleTask(Task* task, bool count_task);
	virtual void AddResult(ldd::protocol::Response* response) ;
    virtual void Done(){};

protected:
	// a number of Job
    boost::shared_ptr<Session> session_;
	int task_num_;
};



class ListJob : public Job{
public:
	ListJob(boost::shared_ptr<Session>msg): Job(msg){

	};

	virtual void AddResult(ldd::protocol::Response* response);
    virtual void Done();
};


}//namespace storage
}//namespace ldd


#endif //__LDD__JOB_H_
