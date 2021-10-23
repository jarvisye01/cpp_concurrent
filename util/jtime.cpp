#include <time.h>

#include "jtime.hpp"

namespace jarvis
{

uint32_t GetUnixTime()
{
    time_t t = time(NULL);
    return t;
}

uint64_t GetMillTime()
{
    timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000 + spec.tv_nsec / 1000;
}

}   // namespace jarvis