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
log4cxx::LoggerPtr Skel::logger;
Skel::Skel():optFlag(0), base_(NULL) 
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
        cout << "opt is " << opt << endl;
        switch (opt)
        {
            case 'r':
                optFlag |= RELOAD;
                break;
            case 'k':
                cout << "flag k" << endl;
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
    logConf_ = config.getConfStr("log_conf");
    logger = log4cxx::LoggerPtr(log4cxx::Logger::getLogger("master"));


    modulesConf_ = config.getConfStr("modules");
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
    //ReadConf();

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

    return ReadConf();
}

int Skel::LoadModules()
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
        Module* module = (*getModule)();
        
        DestroyModule destroyModule;
        destroyModule =(DestroyModule) dlsym(handler, "DestroyModule");
        if(!destroyModule){
            LOG4CXX_FATAL(logger, "DestroyModule error");
            return -1;
        }
        
        ModuleInfo info;
        info.module_ = module;
        info.conf_ = tmpVec[1];
        info.destroy_ = destroyModule;
        info.handler_ = handler;
        modules_.push_back(info); 

        //dlclose(handler);
        tmpVec.clear();
    }
    LOG4CXX_DEBUG(logger, "LoadModules finished!");
    return 0;
}

void Skel::WorkerRun(ModuleInfo* info, const string& conf)
{
    pid_t pid;

    if((pid = fork()) > 0){
        return;
    }   
    
    info->pid = pid;
    Module* module = NULL;
    module = info->module_;
    LOG4CXX_FATAL(logger, "worker run ,conf : " << conf); 
    //child process, will run module in this process
    module->Init(conf);
    module->Run();
}

void Skel::MasterRun()
{
    //init log4cxx 
    try
    {
        PropertyConfigurator::configure(logConf_);
    }
    catch(std::exception& e)
    {
        LOG4CXX_FATAL(logger, "exception:%s "<< e.what());
        exit(1);
    }
    
    LOG4CXX_FATAL(logger,"log4cxx init finish!");
    
    base_ = event_base_new();
    RegisterSignal();
    
    if(LoadModules() < 0){
        return;
    }
    
    LOG4CXX_DEBUG(logger, "modules_.size() " << modules_.size());
    for(int i = 0 ; i < modules_.size(); i ++){
       WorkerRun(&modules_[i], modules_[i].conf_); 
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
        modules_[i].module_->Stop();
        kill(modules_[i].pid , SIGKILL);
    }
    
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

    }
}

void Skel::ReloadHandle()
{
    status_ = SUSPENDING;

    //read some modified conf
    Config config;
    config.init(conf_.c_str());

    modulesConf_ = config.getConfStr("modules");
    
    //stop worker module
    for(int i = 0; i < modules_.size(); i++){
        modules_[i].module_->Stop();
    }

    modules_.clear();

    //restart the modules
    if(LoadModules() < 0){
        LOG4CXX_ERROR(logger, "loadmodules error! exit()");
        exit(-1);
    }
  

    for(int i = 0; i< modules_.size(); i ++){
        WorkerRun(&modules_[i], modules_[i].conf_);
    }

    return;

}

void Skel::SignalHandle(int sig)
{
    LOG4CXX_DEBUG(logger, "in signal handler,sig : "<<sig);

    switch (sig) {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
            Stop();
            break;
        case SIGHUP:
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


