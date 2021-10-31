#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include "util/jrand.hpp"


namespace jarvis
{

namespace jrand
{

pthread_once_t initSeed = PTHREAD_ONCE_INIT;

int RandN(int n)
{
    pthread_once(&initSeed, [] () {
        srand(time(NULL));
    });

    return rand() % n;
}

}  // namespace jrand

}  // namespace jarvis
