#include "util/jdebug.hpp"
#include <cstdint>

#include <unistd.h>

namespace jarvis
{

namespace jdebug
{

void Pause(uint64_t time)
{
    if (time == 0)
        while (true) {}
    else
        usleep(time * 1000);
}

}   // namespace jdebug

}   // namespace jarvis
