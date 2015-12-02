#ifndef __LDD__STORAGE_STORE_MODULE_H_
#define __LDD__STORAGE_STORE_MODULE_H_


#include <string>
#include <ldd/skel/module.h>
#include <ldd/courier/options.h>



namespace ldd{
namespace storage{


class ConfigManager;
class PlantPool;
class DB;
//class NetWorking;

class StoreModule : public ldd::skel::SimpleModule{
public:
	StoreModule();
	~StoreModule();
	virtual bool InitModule(ModuleCtx* ctx);

private:
	bool RunStore();
    void HandleZkError(int error_code);

private:

	ConfigManager*	config_manager_;
	PlantPool*		plant_pool_;
	DB*				db_impl_;

	int				listen_port_;

	int				read_thread_num_;
	int				write_thread_num_;

	
	std::string		zk_host_port_;
	std::string		zk_root_path_;
	
	std::string		db_path_;
    ldd::courier::Options option_;	
	
};





}//namespace storage
}//namespace ldd





#endif //__LDD__STORAGE_STORE_MODULE_H_
