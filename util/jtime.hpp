#ifndef J_TIME_H
#define J_TIME_H
#include <inttypes.h>

namespace jarvis
{

// get unix time of now
uint32_t GetUnixTime();


// get unix mill time of now
uint64_t GetMillTime();

}   // namespace jarvis

#endif