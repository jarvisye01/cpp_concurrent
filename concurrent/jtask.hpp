#ifndef J_TASK_H
#define J_TASK_H

namespace jarvis
{

class JTask
{
public:
    // every task has a id
    virtual int GetTaskID() const = 0;
    virtual int Run() = 0;
};

}   // namespace jarvis

#endif