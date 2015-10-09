#include "../module.h"
#include <string>
#include <iostream>
using namespace std;
class EchoModule: public Module{
    public:
        virtual void Init(const string& conf);
        virtual void Run();
        virtual void Stop();
};

void EchoModule::Init(const string& conf)
{
    
    try
    {
        PropertyConfigurator::configure("examplemodule/echomodulelog.conf");
    }
    catch(std::exception& e)
    {
        LOG4CXX_FATAL(logger, "exception:%s "<< e.what());
        return;
    }
    logger = log4cxx::LoggerPtr(log4cxx::Logger::getLogger("echomodule"));


    LOG4CXX_DEBUG(logger, "echo module initi, conf"<<conf);
}

void EchoModule::Run()
{
    LOG4CXX_DEBUG(logger, "echo module run");
    while(1){
        sleep(2);
        LOG4CXX_DEBUG(logger, "echo module run");
    }
}

void EchoModule::Stop()
{
    LOG4CXX_DEBUG(logger, "echo module run");
}

extern "C" Module* GetModule()
{
    return new EchoModule;    
}

extern "C" void DestroyModule(Module* module)
{
    delete module;
}


