#ifndef J_TIME_H
#define J_TIME_H
#include <inttypes.h>

namespace jarvis
{

// get unix time of now
uint32_t GetUnixTime();


// get unix mill time of now
uint64_t GetMillTime();

// get time year month day hour
void GetTimeDetail(int & year, int & month, int & day, int & hour, int & min, int & sec);

}   // namespace jarvis

#endif