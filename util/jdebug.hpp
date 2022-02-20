#ifndef J_DEBUG_H
#define J_DEBUG_H

#include <cstdint>
#include <stdio.h>

namespace jarvis
{

namespace jdebug
{

#define Printf(fmt, args...)\
    do\
    {\
        printf("%s %d %s " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ## args);\
    }\
    while (0)

#define PrintLn()\
    do\
    {\
        Printf("\n");\
    }\
    while (0)

void Pause(uint64_t time = 0);

}   // namespace jdebug

}   // namespace debug

#endif
