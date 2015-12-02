
#include <zookeeper/zookeeper.h>		//using multithread zk
#include <gflags/gflags.h>
#include <string>
#include <sys/time.h>

//https://code.google.com/p/rapidjson/wiki/UserGuide
#include "../rapidjson/document.h"		// rapidjson's DOM-style API
#include "../rapidjson/prettywriter.h"	// for stringify JSON
#include "../rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output


DEFINE_string(host_port, "127.0.0.1:2181", "zookeeper host port");
DEFINE_string(conf, "./zk_conf.json", "zk conf file(json format)");
DEFINE_string(opt, "write", "opeartion type(clear, write, roles)");


//confs
const char* BASE_CONF_PATH			= "/confs";
const char* REPLICA_NODE_PATH		= "/replicas";
const char* ENGINE_NODE_PATH		= "/engine";
const char* BIN_LOG_PATH			= "/binlog_max";
const char* BUCKET_NODE_PATH		= "/buckets";

//
const char* NODE_LIST_PATH			= "/nodes";
const char* BUCKET_LIST_PATH		= "/buckets";
const char* FARM_LIST_PATH			= "/farms";


const char* COORDINATION			= "/coordinator";
const char* ROLES_PATH				= "/roles";
const char* ALIVE_NODES_LIST_PATH	= "/alive_nodes";

const char* VERSION_PATH			= "/version";


const char* PROXY_NODE_LIST_PATH    = "/proxy_nodes";
const char* NAME_SPACE_LIST_PATH    = "/ns";


const char* FARM_SIZE               = "farm_size";

void clearTree(zhandle_t* handle, const char* conf_path){
    int ret = 0;  int len = 0;
    char path[512];		struct String_vector role_st;
    snprintf(path, sizeof(path), "%s", conf_path);
     ret =zoo_get_children(handle, path, 0, &role_st);
    if (role_st.count > 0 && ret==ZOK){
        for (int i=0 ; i< role_st.count; i++){
            snprintf(path, sizeof(path), "%s/%s", conf_path, role_st.data[i]);
            int ret = zoo_delete(handle,path, -1);
            fprintf(stderr, "clearNodes() %s, ret=%d\n", path, ret);
        }
    }

    len = snprintf(path, 100, "%s", conf_path);
    ret = zoo_delete(handle,path, -1);
    fprintf(stderr, "clearNodesTree() %s, ret=%d\n", path, ret); // delete parent
}

void clearFarms(zhandle_t* handle){
    int ret = 0; 
    char path[512];		struct String_vector role_st;
    snprintf(path, sizeof(path), "%s", FARM_LIST_PATH);
    ret =zoo_get_children(handle, path, 0, &role_st);
    if (role_st.count > 0 && ret==ZOK){
        for (int i=0 ; i< role_st.count; i++){
            snprintf(path, sizeof(path), "%s/%s%s", FARM_LIST_PATH, role_st.data[i], COORDINATION);
            clearTree(handle, path);
            snprintf(path, sizeof(path), "%s/%s%s", FARM_LIST_PATH, role_st.data[i], ROLES_PATH);
            clearTree(handle, path);
            snprintf(path, sizeof(path), "%s/%s%s", FARM_LIST_PATH, role_st.data[i], ALIVE_NODES_LIST_PATH);
            clearTree(handle, path);

            snprintf(path, sizeof(path), "%s/%s", FARM_LIST_PATH, role_st.data[i]);
            int ret = zoo_delete(handle,path, -1);
            fprintf(stderr, "clearFarms() %s, ret=%d\n", path, ret);
        }
    }

    snprintf(path, sizeof(path), "%s", FARM_LIST_PATH);
    ret = zoo_delete(handle,path, -1);
    fprintf(stderr, "clearFarms() %s, ret=%d\n", path, ret);
}

int ClearZkTree(zhandle_t* handle){
	int ret = 0;  int len = 0;
	char path[100];
	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, REPLICA_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_delete(handle, path, -1);

	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, ENGINE_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_delete(handle, path, -1);

	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, BIN_LOG_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_delete(handle, path, -1);

	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, BUCKET_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n");
	}
	ret = zoo_delete(handle, path, -1);

    len = snprintf(path, 100, "%s/%s", BASE_CONF_PATH, FARM_SIZE);
    if(len == 0 ){
        fprintf(stderr, "snprintf ERROR\n");
    }
    ret = zoo_delete(handle, path, -1);

    len = snprintf(path, 100, "%s", BASE_CONF_PATH);
    if(len == 0 ){
        fprintf(stderr, "snprintf ERROR\n");
    }
    ret = zoo_delete(handle, path, -1);

    clearTree(handle,NAME_SPACE_LIST_PATH);
    clearTree(handle, PROXY_NODE_LIST_PATH);
    clearTree(handle, NODE_LIST_PATH);
    clearTree(handle, BUCKET_LIST_PATH);
    clearFarms(handle);

	return 0;
}

// create operation  -----------------------------------------------
int CreateConfsOnZkTree(zhandle_t* handle){
	
	int ret = 0;  int len = 0;
	char path[100];
	char path_buffer[100]; int path_len = 100;

	len = snprintf(path, 100, "%s", BASE_CONF_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	ret = zoo_create(handle, path, "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, 
                    path_buffer, path_len);
	fprintf(stderr, "CreateConfsOnZkTree() %s, ret=%d\n", path, ret);


	len = snprintf(path, 100, "%s%s", BASE_CONF_PATH, REPLICA_NODE_PATH);
	if(len == 0 ){
		fprintf(stderr, "snprintf ERROR\n"); 
	}
	return 0;
}

int CreateNode(zhandle_t* handle, const char* path, const char* value){
    int ret = 0;  
    char path_buffer[100]; int path_len = 100;
    ret = zoo_create(handle, path, value, strlen(value), &ZOO_OPEN_ACL_UNSAFE, 0, 
        path_buffer, path_len);
    fprintf(stderr, "CreateNode() path=%s , value=%s, ret=%d\n", path, value, ret);
    return ret;
}
	
// test  if 


void CreateBucketList(zhandle_t* zhandle, 
                      int bucket_size, 
                      int farm_size){

    CreateNode(zhandle, BUCKET_LIST_PATH, "NULL");

    fprintf(stderr, "create bucket list bucket_size=%d, farm_size=%d\n", 
                bucket_size, farm_size);
    if (bucket_size < farm_size){
        fprintf(stderr, "exit bucket_size=%d, farm_size=%d\n", bucket_size, farm_size);
        exit(1);
    }

    for (int i=0; i<bucket_size; i++){
        int farm_id = i%farm_size;
        char path[200]; char value[200];
        int len = snprintf(path,200, "%s/%d", BUCKET_LIST_PATH, i);
        len = snprintf(value,200, "%d", farm_id);
        CreateNode(zhandle, path, value);
        fprintf(stderr, "CreateBucketList() path=%s, value=%s\n", path, value);
    }
}


static int vstrcmpversion(const void* str1, const void* str2) {
    const char **a = (const char**)str1;
    const char **b = (const char**) str2;
    return strcmp(*a, *b); 
} 

static void SortRolesVersionChildren(char ** data, int32_t count) {
    if (data && count > 0){
        qsort(data, count, sizeof(char*), &vstrcmpversion);
    }
}

std::string TimeToString(time_t now)
{
    char charFormat[30];

   /* struct timeval nowtimeval;
    gettimeofday(&nowtimeval,0);*/

    //time_t now;
    struct tm *timenow;

   // time(&now);
    timenow = localtime(&now);

    sprintf(charFormat,
        "%4.4d%2.2d%2.2d:%2.2d:%2.2d:%2.2d",
        timenow->tm_year + 1900,
        timenow->tm_mon + 1,
        timenow->tm_mday,
        timenow->tm_hour,
        timenow->tm_min,
        timenow->tm_sec/*, nowtimeval.tv_usec/1000*/);

    return charFormat;
}

void GetRolesDesc(zhandle_t* handle, const char* farmid){
    int ret = 0;
    char path[512];
    struct String_vector strings;
    snprintf(path, sizeof(path), "%s/%s%s", FARM_LIST_PATH, farmid, ROLES_PATH);
    ret =zoo_get_children(handle, path, 0, &strings);
  
    if (strings.count > 0 && ret==ZOK){
        SortRolesVersionChildren(strings.data, strings.count);
        char* p = strings.data[strings.count - 1];
        snprintf(path, sizeof(path), "%s/%s%s/%s", 
            FARM_LIST_PATH, farmid, ROLES_PATH, p);
        char value[10240];
        int value_len = 10240;

        struct Stat stat;
        
        ret = zoo_get(handle, path, 0, value, &value_len, &stat);
        value[value_len] = '\0';
        //zoo_get(handle, "/farms/0/roles/version0000000075", 0, value, &value_len, &stat);

        std::string TT = TimeToString(stat.ctime / 1000);
        fprintf(stderr, "\n%s, ctime=%lu, %s\n", path, stat.ctime, TT.c_str());
        fprintf(stderr, "farmid=%s,                           roles=%s\n", farmid, value);

    }
}

void GetRolesofFarms(zhandle_t* handle){
    int ret = 0; 
    char path[512];		struct String_vector role_st;
    snprintf(path, sizeof(path), "%s", FARM_LIST_PATH);
    ret =zoo_get_children(handle, path, 0, &role_st);
    if (role_st.count > 0 && ret==ZOK){
        SortRolesVersionChildren(role_st.data, role_st.count);
        for (int i=0 ; i< role_st.count; i++){
            GetRolesDesc(handle, role_st.data[i]);
            //snprintf(path, sizeof(path), "%s/%s%s", FARM_LIST_PATH, role_st.data[i], ROLES_PATH);

        }
    }
}


void CreateTree(zhandle_t* zhandle_, rapidjson::Document& document){

    int bucket_size = 0;
    int farm_size   = 0;

    for (rapidjson::Document::ConstMemberIterator itr = document.MemberBegin(); 
        itr != document.MemberEnd(); ++itr) {
            const char* base =(itr->name).GetString();

            char path[100]= {0};
            int len = snprintf(path, 100, "/%s", base);
            if(len == 0 ){
                fprintf(stderr, "snprintf ERROR\n"); 
            }
            fprintf(stderr, "paser base node:%s \n", base);
            CreateNode(zhandle_, path, "NULL");

            {
                rapidjson::Type t = itr->value.GetType();
                if (t != rapidjson::kObjectType){
                    fprintf(stderr, "not object type , continue\n");
                    continue;
                }
                const rapidjson::Value& object = itr->value;
                for (rapidjson::Document::ConstMemberIterator itr2 = object.MemberBegin(); 
                    itr2 != object.MemberEnd(); ++itr2) {
                        //key
                        const char* key =(itr2->name).GetString();
                        fprintf(stderr, "paser internal node name:%s \n", key);
                        std::string path = "/";
                        path = path + base + "/" + key;


                        //value
                        const rapidjson::Value& right = itr2->value;
                        assert(right.IsString());
                        const char* value = right.GetString();

                        fprintf(stderr, "paser internal value:%s \n", value);
                        CreateNode(zhandle_, path.c_str(), value);


                        if (strcmp(key, FARM_SIZE) == 0){
                            farm_size = atoi(value);
                        }

                        if (strcmp(key, "buckets") == 0){
                            bucket_size = atoi(value);
                        }

                        std::string base_path = "/";
                        base_path = base_path + base;
                        if (strcmp(base_path.c_str(), FARM_LIST_PATH) == 0 )
                        {
                            std::string coordinator, alive_nodes, roles;
                            coordinator = path  + COORDINATION;
                            alive_nodes = path + ALIVE_NODES_LIST_PATH;
                            roles       = path + ROLES_PATH;
                            fprintf(stderr, "create farms children \n");
                            CreateNode(zhandle_, coordinator.c_str(), "NULL");
                            CreateNode(zhandle_, alive_nodes.c_str(), "NULL");
                            CreateNode(zhandle_, roles.c_str(), "NULL");
                        }
                }
            }


            //count++;	// name
            //count += Traverse(itr->value);
    }
    assert(document.HasMember("confs"));

    CreateBucketList(zhandle_, bucket_size, farm_size);
}

const int file_length = 10240;
//gdb --args ./create_zk_tree -host_port=127.0.0.1:2182 -opt=roles -conf=zk_conf.json.sample 
int main(int argc, char **argv){
    zhandle_t* 				zhandle_ = NULL; 
    google::ParseCommandLineFlags(&argc, &argv, false);
    fprintf(stderr, "./create_zk_tree -host_port=127.0.0.1:2182 -opt=roles -conf=zk_conf.json.sample");

    FILE*fp =   fopen(FLAGS_conf.c_str(), "r");
    if (!fp){
        fprintf(stderr, "fopen file%s error\n", FLAGS_conf.c_str());
        exit(1);
    }
    zhandle_ = zookeeper_init(FLAGS_host_port.data(), NULL, 20000, 0, NULL, 0);
    if (!zhandle_){
        fprintf(stderr, "zookeeper_init error\n");
        exit(1);
    }

    if (strcmp(FLAGS_opt.c_str(), "clear") == 0 ){
        ClearZkTree(zhandle_);
        fprintf(stderr, "clear zk tree\n");
        return 0;
    }else if (strcmp(FLAGS_opt.c_str(), "write") == 0 ){
        fprintf(stderr, "crate zk tree");
        char buffer[file_length] = {0};
        size_t read_n = fread(buffer, sizeof(char), file_length, fp);
        if (read_n == 0 ){
            fprintf(stderr, "fread %d error\n", (int)read_n);
            exit(1);
        }else{
            fprintf(stderr, "buffer: %s\n", buffer);
        }

        rapidjson::Document document;
        if(document.Parse<0>(buffer).HasParseError() ){
            fprintf(stderr, "paser ERROR %s \n", buffer);
            return false;
        }

        CreateTree(zhandle_, document);
    }else if (strcmp(FLAGS_opt.c_str(), "roles") == 0 ){
        GetRolesofFarms(zhandle_);
    }
    

	zookeeper_close(zhandle_);
	fprintf(stderr, "byebye!!!\n");


	return 0;
}


