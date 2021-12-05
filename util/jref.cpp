#include <stddef.h>
#include "util/jref.hpp"

namespace jarvis
{

JUseCount::JUseCount(): count(new int(1))
{}

JUseCount::JUseCount(const JUseCount & c): count(c.count)
{
    AddRef();
}

JUseCount& JUseCount::operator=(const JUseCount & c)
{
    if (this == &c)
        return *this;
    DelRef();
    if (IsOnly())
        delete count, count = NULL;
    count = c.count;
    return *this;
}

JUseCount::~JUseCount()
{
    if (IsOnly())
    {
        delete count;
        return;
    }
    DelRef();
}

inline bool JUseCount::IsOnly()
{
    return *count == 1;
}

inline int JUseCount::AddRef()
{
    return ++(*count);
}

inline int JUseCount::DelRef()
{
    return --(*count);
}

int JUseCount::RefCount()
{
    return *count;
}

};  // namespace jarvis
