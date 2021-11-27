#ifndef J_STRING_H
#define J_STRING_H

#include <string>

namespace jarvis
{

namespace jstring
{

std::string GetRandomString(int n);

std::string TrimString(const std::string & str, const std::string & s);

}  // namespace jstring

}  // namespace jarvis

#endif
