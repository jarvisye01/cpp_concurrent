#include <string>
#include <set>
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

std::string TrimString(const std::string & str, const std::string & s)
{
    if (s.size() == 0)
        return str;
    
    std::set<char> chs;
    chs.insert(s.begin(), s.end());
    int i = 0, j = str.size() - 1;

    while (i <= j)
    {
        if (chs.find(str[i]) != chs.end())
            i++;
        if (chs.find(str[j]) != chs.end())
            j--;
    }
    
    if (i > j)
        return std::string("");

    return str.substr(i, j - i + 1);
}

}  // namespace jstring

}  // namespace jarvis
