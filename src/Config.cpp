
#include "Config.h"

namespace myTask {

std::string findValByKey(const myTask::conf_t &a_conf, const std::string &a_key)
{
    std::string emptyRes = "";

    auto vecb = a_conf.begin();
    while( vecb != a_conf.end() ){
        if ( vecb->name.compare(a_key) == 0 ){
            return vecb->value;
        }
        ++vecb;
    }
    return emptyRes;
}

}

