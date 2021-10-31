#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <functional>
#include "util/jbuffer.hpp"
#include "util/jlog.hpp"
#include "concurrent/jthread.hpp"

using namespace jarvis;

int TestJBuffer()
{
    JBuffer buf(100);

    // 第一次写和读
    const char *str = "dfk;ljsd;lfjdskflsdjowernwenrwenrwlejrljklsjkdfdjkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkbnsmdbfns";
    int n = buf.Write(str, strlen(str));
    printf("write %d bytes\n", n);
    printf("buffer: %s\n", buf.ToString().c_str());
    char buf1[51];
    n = buf.Read(buf1, 50);
    printf("Read %d bytes\n", n);
    printf("buffer: %s\n", buf.ToString().c_str());

    // 第二次写和读
    n = buf.Write(str + 30, 37);
    printf("write %d bytes\n", n);
    printf("buffer: %s\n", buf.ToString().c_str());
    n = buf.Avai();
    char buf2[201];
    printf("read %d bytes\n", n);
    n = buf.Read(buf2, n);
    buf2[n] = '\0';
    printf("buffer: %s\n", buf.ToString().c_str());
    return 0;    
}

int TestJLog()
{
    using namespace jarvis;
    jlog::OpenLog();
    jlog::SetLogLevel(jlog::TRACE);

    time_t t = time(NULL);

    std::shared_ptr<JThread> ptrs[3];
    JMutex mutex;
    JCountLatch count(3, mutex);
    for (int i = 0; i < 3; i++)
    {
        ptrs[i].reset(new JThread(std::function<void(void)>([t, i, &count] () {
            time_t t = time(NULL);
            while (time(NULL) < t + 20)
            {
                TRACE("%d %s %d", 3, "hello", i);
            }
            count.Done();
        })));
        ptrs[i]->Start();
        ptrs[i]->Detach();
    }

    count.Wait();
}

int main(int argc, char ** argv)
{
#ifdef JBUFFER
    TestJBuffer();
#endif
#ifdef JLOG
    TestJLog();
    while (true) {}
#endif
    return 0;
}