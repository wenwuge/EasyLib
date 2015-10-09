#include "skeleton.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fstream>
#include <string.h>
#include "Config.h"
#include <tr1/functional>
#include "easyevent.h"
#include <signal.h>
#include <../common/Strops.h>
#include <dlfcn.h>
#include <assert.h>
#include <sstream>
#include <sys/types.h>
#include<sys/wait.h>
#include <sys/prctl.h>
log4cxx::LoggerPtr Skel::logger(log4cxx::Logger::getLogger("master"));
Skel::Skel():optFlag(0), base_(NULL),autoReboot_(0),
            status_(STARTING)    
{
}

Skel::~Skel()
{
    if(base_){
        event_base_free(base_);
    }
}
int Skel::ParseOpt(int argc, char** argv)
{
    int opt; 
    conf_ = "conf/skel.conf";
    logConf_ = "conf/skellog.conf";
    while((opt = getopt(argc, argv, "rksfc:l:")) != -1)
    {
        switch (opt)
        {
            case 'r':
                optFlag |= RELOAD;
                break;
            case 'k':
                optFlag |= KILL;
                break;
            case 's':
                optFlag |= STATUS;
                break;
            case 'f':
                optFlag |= FORERUN;
                break;
            case 'c':
                conf_ = strdup(optarg);
                break;
            case 'l':
                logConf_ = strdup(optarg);
                break;
            default:
                return -1;
        }
    }


    return  0;
}

void Skel::ChangeProcessName(string name)
{
    prctl(PR_SET_NAME, name.c_str());
}

string Skel::GetProcessNameById(pid_t pid)
{
    //get process exe file path
    FILE *fptr;
    bool bret = false;
    char cmd[255] = {'\0'};
    char pathName[512] ={'\0'};
    sprintf(cmd,"readlink /proc/%d/exe",pid);
    if((fptr = popen(cmd,"r")) != NULL)
    {
        if(fgets(pathName,512,fptr) != NULL)
        {
        }
    }
    pclose(fptr);


    //get process name
    char name[256]={'\0'};
    const char* pos = strrchr(pathName,'/');
    if(pos == 0)
    {
        strcpy(name,pathName);
    }else{
        strcpy(name,pos+1);
    }
    return name;
}

bool Skel::ReloadOpt()
{
    return optFlag&RELOAD != 0;
}

bool Skel::KillOpt()
{
    return ((int)(optFlag&KILL)) != 0;
}

bool Skel::StatusOpt()
{
    return ((int)(optFlag&STATUS)) != 0;
}

bool Skel::ForeRunOpt()
{
    return ((int)(optFlag&FORERUN)) != 0;
}

int Skel::ReadConf()
{
    Config config;

    config.init(conf_.c_str());
    #if 0
    pidFilePath = config.getConfStr("pid_file");
    cout << "pidFilePath : " << pidFilePath << endl;
    #else
    pidFilePath = "./pid";
    #endif
    if(config.checkIsKeyExist(0,"log_conf")){
        logConf_ = config.getConfStr("log_conf");
    }
    


    if(config.checkIsKeyExist(0,"modules")){
        modulesConf_ = config.getConfStr("modules");
    }
   
    if(config.checkIsKeyExist(0,"autoreboot")){

        autoReboot_ = config.getConfInt("autoreboot");
    }

    if(config.checkIsKeyExist(0,"reboottimeout")){
        rebootTimeout_ = config.getConfInt("reboottimeout");

    }
    if(!KillOpt()&&!StatusOpt()&&!ReloadOpt() ){    
        try
        {
            PropertyConfigurator::configure(logConf_);
        }
        catch(std::exception& e)
        {
            LOG4CXX_FATAL(logger, "exception:%s "<< e.what());
            exit(1);
        }
        
       // logger = log4cxx::LoggerPtr(log4cxx::Logger::getLogger("master"));
        LOG4CXX_FATAL(logger,"log4cxx init finish!");
        LOG4CXX_DEBUG(logger, "modules: "<<modulesConf_);
        LOG4CXX_DEBUG(logger, "autoreboot: "<<autoReboot_);
        LOG4CXX_DEBUG(logger, "reboottimeout: "<<rebootTimeout_);
    }
    return 0;
}

int Skel::Status(pid_t& pid)
{
    if(access(pidFilePath.c_str(),0) < 0){
        cout << "no pid file exsit" << endl;
        return DEAD;
    }

    ifstream input(pidFilePath.c_str());
    input>>pid;
    
    //judge the pid whether alived
    char procPath[64]={0};
    snprintf(procPath, 64, "/proc/%d", pid);
    if(access(procPath, 0) < 0){
        cout << "process "<< pid << " not exsited, maybe have been killed" <<endl;
        return DEAD;
    }
    
    if(GetProcessNameById(pid).find("skel") == string::npos){
        cout << "pid " << pid << " name is " << GetProcessNameById(pid) << endl;
        return DEAD;
    }
    return ALIVE;
}

void Skel::Reload()
{
    pid_t pid;

    if(Status(pid) == ALIVE){
        //send SIGHUP signal to the process 
        kill(pid, SIGHUP);
    }
    return;
}

void Skel::Kill()
{
    pid_t pid;
    if(Status(pid) == ALIVE){
        //send term signal to the process 
        kill(pid, SIGTERM);
    }
    return;
}

int Skel::Init(int argc, char ** argv)
{
    
    if(ParseOpt(argc, argv)){
        return -1;
    }
    ReadConf();

    if(ReloadOpt()){
        Reload();
        return -1;
    }
    
    if(KillOpt() == true){
        Kill();
        return -1;
    }

    if(StatusOpt()){
        pid_t pid;
        if(Status(pid) == ALIVE){
            cout << "skel alive" << endl;
        }else{
            cout << "skel dead" << endl;
        }
        return -1;
    }

    return 0;
   // return ReadConf();
}

int Skel::CheckModules()
{
    vector<string> vec;
    
    EasyString::split(modulesConf_, ";", vec); 
    ostringstream os;
    LOG4CXX_DEBUG(logger, "find" << vec.size() << "modules" << " modules: " << modulesConf_);

    for(int i=0; i < vec.size(); i++){
        vector<string> tmpVec;
        EasyString::split(vec[i], ":", tmpVec);

        LOG4CXX_DEBUG(logger, "Module name:" << tmpVec[0] << " conf" << tmpVec[1] );

        //load one module
        void* handler = dlopen(tmpVec[0].c_str(), RTLD_NOW);
        if(!handler){
            LOG4CXX_FATAL(logger, "dlopen error," << dlerror());
            return -1;
        }
        
        GetModule getModule;
        getModule =(GetModule) dlsym(handler, "GetModule");
        if(!getModule){
            LOG4CXX_FATAL(logger, "GetModule error");
            return -1;
        }
        
        DestroyModule destroyModule;
        destroyModule =(DestroyModule) dlsym(handler, "DestroyModule");
        if(!destroyModule){
            LOG4CXX_FATAL(logger, "DestroyModule error");
            return -1;
        }

        dlclose(handler);
        tmpVec.clear();
    }
    LOG4CXX_DEBUG(logger, "CheckModules finished!");
    return 0;
}

int Skel::WorkerRun(const string & conf)
{
    pid_t pid;

    if((pid = fork()) > 0){
        ModuleInfo info;
        info.pid = pid;
        modules_.push_back(info);
        return 0;
    }   
    

    Module* module = NULL;

    vector<string> tmpVec;
    EasyString::split(conf, ":", tmpVec);

    LOG4CXX_DEBUG(logger, "Module name:" << tmpVec[0] << " conf" << tmpVec[1] );

    //load one module
    void* handler = dlopen(tmpVec[0].c_str(), RTLD_NOW);
    if(!handler){
        LOG4CXX_FATAL(logger, "dlopen error," << dlerror());
        return -1;
    }
    
    GetModule getModule;
    getModule =(GetModule) dlsym(handler, "GetModule");
    if(!getModule){
        LOG4CXX_FATAL(logger, "GetModule error");
        return -1;
    }
    module = (*getModule)();
    
    DestroyModule destroyModule;
    destroyModule =(DestroyModule) dlsym(handler, "DestroyModule");
    if(!destroyModule){
        LOG4CXX_FATAL(logger, "DestroyModule error");
        return -1;
    }
    
    //child process, will run module in this process
    module->Init(conf);
    module->Run();
}

void Skel::MasterRun()
{
    //init log4cxx 

    
    base_ = event_base_new();
    RegisterSignal();
    ChangeProcessName("Monitor"); 

    if(CheckModules() < 0){
        LOG4CXX_FATAL(logger, "checkmodules error happen");
        return;
    }
    
    vector<string> vec;
    
    EasyString::split(modulesConf_, ";", vec); 
    LOG4CXX_DEBUG(logger, "find modules num: " << vec.size());
    for(int i = 0 ; i < vec.size(); i ++){
       WorkerRun(vec[i]); 
    }

    //will listen signal and timers
    event_base_dispatch(base_);
    LOG4CXX_FATAL(logger, "master stop");
}

void Skel::DaemonRun()
{
    //run as one daemon program
    daemon(1,0);

    pid_t pid = getpid();
    //record the pid in the pidFile
    ofstream pidFile(pidFilePath.c_str());

    pidFile << pid;
    pidFile.close();

    MasterRun();

}

void Skel::ForeRun()
{
    MasterRun();
}

int Skel::InitModules()
{
    return  0;
}

void Skel::Stop()
{
    for(int i = 0 ; i < modules_.size(); i++){
        //modules_[i].module_->Stop();
        //record the pid to be killed
        dropPid_.push_back(modules_[i].pid);
        LOG4CXX_FATAL(logger, "send SIGKILL to " << modules_[i].pid);
        kill(modules_[i].pid , SIGKILL);

    }
   
    //exit the master process
    if(exiting_)
        exit(0);
}

void Skel::WaitChildren()
{
    while(true){
        int status = 0;
        pid_t pid;
        
        bool needKillChildren = false;
        pid = waitpid(-1, &status, WNOHANG);
        if(pid <= 0){
            return;
        }
        
        LOG4CXX_DEBUG(logger, "waitpid pid is " << pid);
        if(WIFEXITED(status)){
            if (WEXITSTATUS(status) != 0){
                needKillChildren = true;
            }
        }else if(WIFSIGNALED(status)){
            if(WTERMSIG(status) != SIGTERM){
                needKillChildren = true;
            }
            
        }else if(WCOREDUMP(status)){
            needKillChildren = true;
        }else{
            continue;
        }

        if(needKillChildren){
            kill(-pid, SIGKILL);
        }
        
        vector<pid_t>::iterator it;
        it = find(dropPid_.begin(), dropPid_.end(), pid);
        //not int the droppid vector, the process maybe meet some problem, pull it up again
        if(it == dropPid_.end()){
            LOG4CXX_DEBUG(logger, "autoReboot_: "<<autoReboot_ << " status: " << status_);
            //need reboot the process,set one reboot timer
            if(autoReboot_&&status_ != SUSPENDING){
               
               TimerEvent* tEvent_ = new TimerEvent(base_);
               struct timeval timeout;
               timeout.tv_sec = rebootTimeout_;
               tEvent_->AsyncWait(std::tr1::bind(&Skel::ReloadHandle , this), 
                            timeout);
               timerEvents_.push_back(tEvent_);
               status_ = SUSPENDING;
            }
        }else{
            dropPid_.erase(it);
        }
            
    }
}

void Skel::ReloadHandle()
{
    //set this status for stop the master libevent work
    status_ = SUSPENDING;

    //read some modified conf
    Config config;
    config.init(conf_.c_str());

    modulesConf_ = config.getConfStr("modules");
    
    //stop worker module

    for(int i = 0; i < modules_.size(); i++){
        dropPid_.push_back(modules_[i].pid);
        //modules_[i].module_->Stop();
        kill(modules_[i].pid,SIGKILL);
        LOG4CXX_DEBUG(logger, "send SIGTERM TO PID "<<modules_[i].pid);
    }

    modules_.clear();

    //restart the modules
    if(CheckModules() < 0){
        LOG4CXX_ERROR(logger, "loadmodules error! exit()");
        exit(-1);
    }
  

    vector<string> vec;
    
    EasyString::split(modulesConf_, ";", vec); 
    LOG4CXX_DEBUG(logger, "find modules num: " << vec.size());
    for(int i = 0 ; i < vec.size(); i ++){
       WorkerRun(vec[i]); 
    }

    status_ = RUNNING;
    return;

}

void Skel::SignalHandle(int sig)
{
    LOG4CXX_DEBUG(logger, "in signal handler,sig : "<<sig);

    switch (sig) {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
            //manual control, not reload need 
            exiting_ = true;
            Stop();
            break;
        case SIGHUP:
            exiting_ = false;
            //Stop();
            ReloadHandle();
            break;
        case SIGCHLD:
            WaitChildren();
            break;
        default:
            return;
    }
    signalEvent->AsyncWait(std::tr1::bind(&Skel::SignalHandle, this, std::tr1::placeholders::_1));
    return;
}

void Skel::RegisterSignal()
{
    signalEvent.reset(new SignalEvent(base_));

    signalEvent->Add(SIGINT);
    signalEvent->Add(SIGTERM);
    signalEvent->Add(SIGHUP);
    signalEvent->Add(SIGQUIT);
    signalEvent->Add(SIGCHLD);

    signalEvent->AsyncWait(std::tr1::bind(&Skel::SignalHandle, this, std::tr1::placeholders::_1));
}

int Skel::Start()
{
    //signal register
    
    if(InitModules()){
        cout << "init modules err!" << endl;
        return -1;
    }
    
    if(ForeRunOpt()){
        ForeRun();
    }else{
        DaemonRun();
    }
    return 0; 
}


