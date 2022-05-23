
// TODO

#include "Config.h"

namespace conf {

std::string getParam( const myTask::conf_t &a_conf, const std::string &a_name )
{
    std::string emptyRes = "";

    auto vecb = a_conf.begin();
    while( vecb != a_conf.end() ){
        if ( vecb->name.compare(a_name) == 0 ){
            return vecb->value;
        }
        ++vecb;
    }
    return emptyRes;
}

}

