#ifndef J_BUFFER_H
#define J_BUFFER_H

#include <string>

namespace jarvis
{

// no thread safe
class JBuffer
{
public:
    JBuffer(int sz = 1024);
    JBuffer(const JBuffer &);
    JBuffer& operator=(const JBuffer &);
    ~JBuffer();
    int Write(const void *buf, int n);
    int Read(void *buf, int n);
    int Reset();
    int WriteTo(int fd);
    inline int Size() const;
    inline int Avai() const;
    inline int Rest() const;
    std::string ToString() const;
private:
    int size;
    char *buffer;
    int rp, wp;
    int avai;
};

}   // namespace jarvis

#endif