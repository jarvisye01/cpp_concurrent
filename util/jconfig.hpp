#ifndef J_CONFIG_H
#define J_CONFIG_H

#include <bits/types/FILE.h>
#include <cstdint>
#include <string>
#include <stdio.h>

namespace jarvis
{

// not thread safe
class JConfig
{
public:
    static int Init(const std::string & path);
    static int Done();
    static uint64_t GetInt(const std::string & key);
    // static float GetFloat(const std::string & key);
    static std::string GetString(const std::string & key);
private:
    static std::string GetKey(const std::string & key);
    static FILE *file;
};

}   // namespace jarvis

#endif
