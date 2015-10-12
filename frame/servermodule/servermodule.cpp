#include "../module.h"
#include <string>
#include <iostream>
#include "../../net/threadpool.h"
using namespace std;
class ServerModule: public Module{
    public:
        virtual void Init(const string& conf);
        virtual void Run();
        virtual void Stop();
        ServerModule(Uint16 thread_num);
        ~ServerModule();
    private:
        void myjob(TcpMessageDescPtr& tcpPtr);
    private:
        Threadpool pool;
};

ServerModule::ServerModule(Uint16 thread_num):pool(thread_num)
{
}

ServerModule::~ServerModule()
{
    pool.Stop();
}

void ServerModule::myjob(TcpMessageDescPtr& tcpPtr)
{
    
    char tmp[1024];
    cout << "in process Data!" << endl;
    memcpy(tmp, tcpPtr->GetBufPtr(), tcpPtr->GetDataLen());

    tmp[tcpPtr->GetDataLen()]='\0';

    cout << " receive data: " << tmp << endl;

    Thread::Send(tcpPtr , tmp, tcpPtr->GetDataLen());
}

void ServerModule::Init(const string& conf)
{
    
    try
    {
        PropertyConfigurator::configure("servermodule/ServerModulelog.conf");
    }
    catch(std::exception& e)
    {
        LOG4CXX_FATAL(logger, "exception:%s "<< e.what());
        return;
    }
    logger = log4cxx::LoggerPtr(log4cxx::Logger::getLogger("ServerModule"));


    LOG4CXX_DEBUG(logger, "server module initi, conf"<<conf);
    
    pool.RegistersJob(std::tr1::bind(&ServerModule::myjob, this, std::tr1::placeholders::_1));


}

void ServerModule::Run()
{
    LOG4CXX_DEBUG(logger, "server module run");
    pool.Start(22222);
}

void ServerModule::Stop()
{
    LOG4CXX_DEBUG(logger, "server module run");
}

extern "C" Module* GetModule()
{
    return new ServerModule(5);    
}

extern "C" void DestroyModule(Module* module)
{
    delete module;
}


