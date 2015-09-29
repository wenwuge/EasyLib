#ifndef __MODULE
#define __MODULE
#include <string>
#include <log4cxx/logger.h>
using namespace log4cxx;
using namespace std;
class Module{
    public:
        virtual void Init(const string& conf) = 0;
        virtual void Run() = 0;
        virtual void Stop() = 0;
        virtual ~Module(){};
        LoggerPtr& Logger();
};
#endif

