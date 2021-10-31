#include <time.h>

#include "util/jtime.hpp"

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

void GetTimeDetail(int & year, int & month, int & day, int & hour, int & min, int & sec)
{
    time_t t = time(NULL);
    tm *tm = localtime(&t);
    year = tm->tm_year + 1900;
    month = tm->tm_mon + 1;
    day = tm->tm_mday;
    hour = tm->tm_hour;
    min = tm->tm_min;
    sec = tm->tm_sec;
}

}   // namespace jarvis