#include "../module.h"
#include <string>

class EchoModule: public Module{
    public:
        virtual void Init(const string& conf);
        virtual void Run();
        virtual void Stop();
};

void EchoModule::Init(const string& conf)
{
    LOG4CXX_DEBUG(Logger(), "echo module initi, conf"<<conf);
}

void EchoModule::Run()
{
    LOG4CXX_DEBUG(Logger(), "echo module run");
}

void EchoModule::Stop()
{
    LOG4CXX_DEBUG(Logger(), "echo module run");
}

extern "C" Module* GetModule()
{
    return new EchoModule;    
}

extern "C" void DestroyModule(Module* module)
{
    delete module;
}


