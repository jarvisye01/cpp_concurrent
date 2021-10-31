#include <unistd.h>
#include <fcntl.h>
#include <functional>
#include <pthread.h>
#include "util/jlog.hpp"
#include "util/jtime.hpp"
#include "concurrent/jthread.hpp"

namespace jarvis
{

namespace jlog
{

int WriteToFile(std::shared_ptr<JBuffer> & buffer);

// =============global val============
// log level
LOGLEVEL glevel;

// log file
const char *logDir = "/root/log/";

// Log format [time level file line function message] 
const char *fmtStrTRACE = "%s TRACE %s\n";
const char *fmtStrDDEBUG = "%s DEBUG %s\n";
const char *fmtStrINFO = "%s INFO %s\n";
const char *fmtStrWARN = "%s WARN %s\n";
const char *fmtStrERROR = "%s ERROR %s\n";

// input buffer
std::shared_ptr<JBuffer> logBuf;
JSyncQueue<std::shared_ptr<JBuffer>> logBufQ;
JMutex bMutex;   // guard logBuf
std::shared_ptr<JThread> writeThread;
std::shared_ptr<JThread> timeThread;
pthread_mutex_t t = PTHREAD_MUTEX_INITIALIZER;
// =============global val============

int OpenLog()
{
    glevel = INFO;
    logBuf.reset(new JBuffer(1024 * 10));
    
    std::string name("back log thread");
    writeThread.reset(new JThread(std::function<void(void)>([] () {
        while (true)
        {
            int sz = logBufQ.Size();
            if (sz != 0)
                printf("queue size: %d\n", sz);
            std::shared_ptr<JBuffer> buf = logBufQ.Pop();
            if (buf->Avai() > 0)
            {
                WriteToFile(buf);
            }
        }
    }), name));
    writeThread->Start();
    writeThread->Detach();

    std::string timeName = "time thread";
    timeThread.reset(new JThread(std::function<void(void)>([] () {
        while (true)
        {
            sleep(1);
            std::shared_ptr<JBuffer> tmp(new JBuffer(1024 * 10));
            {
                JMutexHelper helper(bMutex);
                if (logBuf->Avai() > 0)
                {
                    tmp.swap(logBuf);
                    logBufQ.Push(tmp);
                }
            }
        }
    }), timeName));
    timeThread->Start();
    timeThread->Detach();  
    return 0;
}

LOGLEVEL SetLogLevel(LOGLEVEL l)
{
    auto old = glevel;
    glevel = l;
    return old;
}


int WriteToBuf(const char *buf, int n)
{
    JMutexHelper helper(bMutex);
    int wn = 0, rest = n;
    while (wn < n)
    {
        int tn = logBuf->Write(buf + wn, rest);
        wn += tn;
        rest -= tn;
        if (tn < rest)
        {
            // 写满了一个buffer仍然未写完，将这个buf送入队列
            logBufQ.Push(logBuf);
            logBuf.reset(new JBuffer(1024 * 10));
        }
    }
    return wn;
}

int WriteToFile(std::shared_ptr<JBuffer> & buffer)
{
    int year, mon, day, hour, min, sec;
    GetTimeDetail(year, mon, day, hour, min, sec);
    char path[64];
    int n = sprintf(path, "%s%04d%02d%02d%02d.log", logDir, year, mon, day, hour);
    path[n] = '\0';
    int fd = open(path, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
        return -1;
    buffer->WriteTo(fd);
    close(fd);
    return 0;
}

int Log(LOGLEVEL l, const char *buf, int n)
{
    if (l < glevel)
        return 0;   // 日志级别不够，不进行写入
    char log[2048];
    int year, mon, day, hour, min, sec;
    GetTimeDetail(year, mon, day, hour, min, sec);
    char time[32];
    int sz = sprintf(time, "%02d:%02d:%02d", hour, min, sec);
    time[sz] = '\0';
    switch (l)
    {
    case TRACE:
        sz = snprintf(log, 2048, fmtStrTRACE, time, buf);
        log[sz] = '\0';
        WriteToBuf(log, sz);
        break;
    case DEBUG:
        sz = snprintf(log, 2048, fmtStrDDEBUG, time, buf);
        log[sz] = '\0';
        WriteToBuf(log, sz);
        break;
    case INFO:
        sz = snprintf(log, 2048, fmtStrINFO, time, buf);
        log[sz] = '\0';
        WriteToBuf(log, sz);
        break;
    case WARN:
        sz = snprintf(log, 2048, fmtStrWARN, time, buf);
        log[sz] = '\0';
        WriteToBuf(log, sz);
        break;
    case ERROR:
        sz = snprintf(log, 2048, fmtStrERROR, time, buf);
        log[sz] = '\0';
        WriteToBuf(log, sz);
        break;
    default:
        break;
    }
    return 0;
}

// 最好是直接重定向，减少不必要的复制
int LogTrace(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, 1024, fmt, ap);
    buf[n] = '\0';
    va_end(ap);
    return Log(TRACE, buf, n);
}

int LogDebug(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, 1024, fmt, ap);
    buf[n] = '\0';
    va_end(ap);
    return Log(DEBUG, buf, n);
}

int LogInfo(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, 1024, fmt, ap);
    buf[n] = '\0';
    va_end(ap);
    return Log(INFO, buf, n);
}

int LogWarn(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, 1024, fmt, ap);
    buf[n] = '\0';
    va_end(ap);
    return Log(WARN, buf, n);
}

int LogError(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, 1024, fmt, ap);
    buf[n] = '\0';
    va_end(ap);
    return Log(ERROR, buf, n);
}

}  // jlog

}  // jarvis