#ifndef SKEL_H
#define SKEL_H
//one frame for server, include one master process, one worker process,
//in version1.0,the follow features will be contained:
//1support foreground and demonground run
//2support worker alive monitor
//3support module loaded
//4support reload without reboot
//5users task will be insert by module way
//6signal process
#include <stdint.h>
#include <iostream>
#include <vector>
#include <tr1/memory>
#include "easyevent.h"
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <boost/format.hpp>
#include <unistd.h>
#include <string.h>
#include "module.h"
#include <dlfcn.h>
#include <algorithm>
using namespace log4cxx;
using namespace std;
using namespace boost;

typedef Module* (*GetModule)();
typedef void (*DestroyModule)(Module* module);

struct ModuleInfo {
    ModuleInfo():module_(NULL),destroy_(NULL),handler_(NULL),pid(-1){}
    Module* module_;
    string  conf_;
    DestroyModule destroy_;
    void* handler_;
    pid_t pid;
};

class Skel{
    enum{
        STARTING=0,
        RUNNING=1,
        SUSPENDING=2
    };
    enum{
        RELOAD = 0X1,
        KILL = 0X2,
        STATUS = 0X4,
        FORERUN = 0X8
    };

    enum{
        ALIVE=0,
        DEAD = 1
    };
    public:
       Skel();
       ~Skel();
       int Init(int argc, char** argv); 
       int Start();
       void Reload();
       void SignalHandle(int sig);

       int WorkerRun(const string & conf);
       void MasterRun();
       static log4cxx::LoggerPtr logger;
    private:
       void DaemonRun();
       void ForeRun();
       int LoadModules();
       int CheckModules();
       int InitModules();
       int RunModule();
       int ParseOpt(int argc, char** argv);
       int ReadConf();
       void RegisterSignal();

       //Stop
       void Stop();
       void KillWorker();

       //opt
       bool ReloadOpt();
       bool KillOpt();
       bool StatusOpt();
       bool ForeRunOpt();
       void WaitChildren();

       //skel operation
       void Kill();
       int  Status(pid_t& pid);
       void ReloadHandle();
       
       //process relative
       void ChangeProcessName(string name);
       string GetProcessNameById(pid_t pid);
    private:
       string conf_;  
       string logConf_;
       string pidFilePath;
       string modulesConf_;
       uint8_t optFlag;
       uint8_t status_;
       uint8_t exiting_;
       int64_t autoReboot_;
       int64_t rebootTimeout_;
       vector<pid_t> dropPid_;
       vector<TimerEvent*> timerEvents_;
       vector<ModuleInfo> modules_;
       pid_t masterPid;
       std::tr1::shared_ptr<SignalEvent> signalEvent;
       struct event_base *  base_;

};
#endif
