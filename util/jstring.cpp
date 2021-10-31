#include "util/jstring.hpp"
#include "util/jrand.hpp"

namespace jarvis
{

namespace jstring
{

const char *letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

std::string GetRandomString(int n)
{
    char buf[n + 1];
    for (int i = 0; i < n; i++)
    {
        buf[i] = letters[jrand::RandN(62)];
    }
    buf[n] = '\0';
    return std::string(buf);
}

}  // namespace jstring

}  // namespace jarvis