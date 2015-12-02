
#ifndef __LDD__STORAGE_PLANT_POOL_H_
#define __LDD__STORAGE_PLANT_POOL_H_

#include "thread_manager.h"
#include "plant.h"


namespace ldd{
namespace storage{


class PlantPool{
public:
	PlantPool(int read_thread_num, int write_thread_num)
		: read_thread_num_(read_thread_num), write_thread_num_(write_thread_num)
		,tm1_(NULL), read_job_(NULL), write_job_(NULL){

	}
	~PlantPool(){
		// keep the sequence of deletes
		LOG(INFO)<<"~PlantPool()";
        for (int i=0; i < read_thread_num_; i++){
            read_job_[i].Stop();
             LOG(ERROR)<<"~PlantPool() read stop i="<<i;
        }
        delete[] read_job_;

        for (int i=0; i < write_thread_num_; i++){
            write_job_[i].Stop();
            LOG(ERROR)<<"~PlantPool() write stop i="<<i;
        }
		
		delete[] write_job_;
		delete	tm1_;
	}
public:
	void SetUpPlantPool(){
		if (read_thread_num_ ==0 ){
			read_thread_num_ =1;
		}

		if (write_thread_num_ == 0 ){
			write_thread_num_  = 1;
		}
		
		tm1_		= new ThreadManager(read_thread_num_ +  write_thread_num_);						//should new object
		read_job_	= new ReadRequestProcessPlant[read_thread_num_];
		for (int i=0; i < read_thread_num_; i++){
			tm1_->append(&(read_job_[i]));
            LOG(INFO)<<"SetUpPlantPool() add read i="<<i;
		}

		write_job_ = new WriteRequestProcessPlant[write_thread_num_];
		for (int i=0; i < write_thread_num_; i++){
			tm1_->append(&write_job_[i]);
            LOG(INFO)<<"SetUpPlantPool() add write i="<<i;
		}
		LOG(INFO)<<"SetUpPlantPool() set write plant num="<<write_thread_num_;
		LOG(INFO)<<"SetUpPlantPool() set read plant num="<<read_thread_num_;
	}
private:
	int							read_thread_num_; 
	int							write_thread_num_;
	ThreadManager*				tm1_;
	ReadRequestProcessPlant*	read_job_;
	WriteRequestProcessPlant*	write_job_;
};


}//namespace storage
}//namespace ldd







#endif //__LDD__STORAGE_PLANT_H_
