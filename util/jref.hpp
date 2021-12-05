#ifndef J_REF_H
#define J_REF_H

namespace jarvis
{

class JUseCount
{
public:
    JUseCount();
    JUseCount(const JUseCount & c);
    JUseCount& operator=(const JUseCount & c);
    ~JUseCount();

    bool IsOnly();
    int AddRef();
    int DelRef();
    int RefCount();
private:
    int *count;
};
    
};  // namespace jarvis

#endif
