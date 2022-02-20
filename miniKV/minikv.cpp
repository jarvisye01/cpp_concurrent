#include "miniKV/minikv.hpp"
#include <string>

void MinKv::Get(const std::string & key, std::string & val)
{
    val = syncMap.Get(key);
}

void MinKv::Set(const std::string & key, const std::string & val)
{
    syncMap.Insert(key, val);
}
