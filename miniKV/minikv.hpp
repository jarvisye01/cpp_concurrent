#ifndef J_MINIKV_H
#define J_MINIKV_H

#include <string>
#include "concurrent/jmap.hpp"

class MinKv
{
public:
    void Get(const std::string & key, std::string & val);
    void Set(const std::string & key, const std::string & val);
private:
    jarvis::JSyncMap<std::string, std::string> syncMap; 
};

#endif
