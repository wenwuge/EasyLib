#include "module.h"
#include "skeleton.h"
LoggerPtr& Module::Logger()
{
    return Skel::logger;
}
