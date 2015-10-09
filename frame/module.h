#ifndef __MODULE
#define __MODULE
#include <string>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
using namespace log4cxx;
using namespace std;
//using namespace boost;
class Module{
    public:
        Module(string logname = "test"):logger(log4cxx::Logger::getLogger(logname)){};
        virtual void Init(const string& conf) = 0;
        virtual void Run() = 0;
        virtual void Stop() = 0;
        virtual ~Module(){};
    protected:
        log4cxx::LoggerPtr logger;
};
#endif

