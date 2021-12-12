#ifndef J_BUFFER_H
#define J_BUFFER_H

#include <stddef.h>
#include <string>
#include <exception>

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

namespace jutil
{

// 重新设计buffer，放到jutil空间避免冲突，这次需要考虑异常体系

/*
 * 越界异常
 */
class JOutOfBoundExp: public std::exception
{
public:
    const char* what() const noexcept;
};

/*
 * 整形操作数异常
 */
class JIntOptExp: public std::exception
{
public:
    JIntOptExp(int opp);
    const char* what() const noexcept; 
    int GetIntOpt() const;
private:
    int op;
};

/*
 * Buffer是一个buffer接口，可以完成基本的write和read操作
 */
class JBuffer
{
public:
    virtual int Write(const void * buf, size_t sz) = 0;
    virtual int Read(void * buf, size_t sz) = 0;
    virtual void Reset() {};
    virtual ~JBuffer() {}
};

/*
 * BufferBase一个buffer基类，实现基本的读写
 * 内部基于一个char*数组，可以自动调整内部空间
 */
class JBufferBase: public JBuffer
{
public:
    JBufferBase(int initSize = 1);
    JBufferBase(const JBufferBase & jbuf);
    JBufferBase& operator=(const JBufferBase & jbug);
    virtual ~JBufferBase();
    int Write(const void * buf, size_t sz);
    int Read(void * buf, size_t sz);
    void Reset();
    int Size() const;
    int Capacity() const;
private:
    // 保证内部有足够的空间写入n个数据
    void EnsureSpace(int n);
    // 将数据移动到data的前面
    void Move();
    // 扩展/释放空间
    void Expand(int n);
    size_t rp;
    size_t wp;
    size_t size;
    char * data;
};

/*
 * IOBuffer除了基本的读写，还实现了将buffer中的东西写到相应的fd中去
 * 并没有直接实现WriteTo和ReadFrom是因为考虑到不同的fd类型底层实现还是有差别的
 */
class JIOBufferBase: public JBufferBase
{
public:
    virtual ~JIOBufferBase() {}
    // 将buffer数据写到相应地方
    virtual int WriteTo(size_t sz) = 0;
    // 从相应地方读数据到buffer
    virtual int ReadFrom(size_t sz) = 0;
};

/*
 * GenIOBufferBase实现了向handle中进行数据的读写，可以作为通用的基类
 * 并且实现了空间的自适应
 */
class JGenIOBufferBase: public JIOBufferBase
{
public:
    JGenIOBufferBase(int handle = -1);
    ~JGenIOBufferBase();

    int WriteTo(size_t sz);
    int ReadFrom(size_t sz);
private:
    int handle;
};

/*
 * 专用于文件的buffer
 */
class JFileBuffer: public JIOBufferBase
{
};

/*
 * 专用于网络的buffer
 * WriteTo和RecvFrom跟文件buffer实现不同
 */
class JNetBuffer: public JIOBufferBase
{
};

};

}   // namespace jarvis

#endif
