#include <execinfo.h>
#include <stdlib.h>
#include <glog/logging.h>
#include <ostream>

namespace ldd { namespace net {

/* 打印调用栈的最大深度 */
#define DUMP_STACK_DEPTH_MAX 16

/* 打印调用栈函数 */
void dump_trace(const std::string &func_name)
{
    void *stack_trace[DUMP_STACK_DEPTH_MAX] = {0};
    char **stack_strings = NULL;
    int stack_depth = 0;
    int i = 0;

    /* 获取栈中各层调用函数地址 */
    stack_depth = backtrace(stack_trace, DUMP_STACK_DEPTH_MAX);
    
    /* 查找符号表将函数调用地址转换为函数名称 */
    stack_strings = (char **)backtrace_symbols(stack_trace, stack_depth);
    if (NULL == stack_strings) {
        LOG(ERROR) << "Memory is not enough while dump Stack Trace, func name: "
            << func_name;
        return;
    }

    /* 打印调用栈 */
    std::ostringstream os;
    os << func_name << " back trace: \n";
    for (i = 0; i < stack_depth; ++i) {
        os << "\t[" << i << "] " << stack_strings[i] << "\n";
    }

    LOG(INFO) << os.str();

    /* 获取函数名称时申请的内存需要自行释放 */
    free(stack_strings);
    stack_strings = NULL;

    return;
}

} /*namespace net*/
} /*namespace ldd*/

