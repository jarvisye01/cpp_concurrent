#include <stdio.h>
#include <netdb.h>
#include "network/jserver.hpp"
#include "network/jeventloop.hpp"
#include "util/jlog.hpp"

using namespace std;
using namespace jarvis;

int TestEventCallBack(JTcpConn *conn, JEventLoop *el)
{
    // read data
    char buf[1024];
    int ret = conn->Recv(buf, -1, NULL);
    buf[ret] = '\0';

    std::string msg = "hello, ";
    msg = msg + std::string(buf + 6);
    int len = msg.size() + 6;
    len = htonl(len);
    conn->Send("ye", 2, NULL);
    conn->Send((char *)&len, 4, NULL);
    conn->Send(msg.c_str(), msg.size(), NULL);
    el->GetEpoller()->AddEvent(conn->GetFd(), EPOLLOUT);
    return 0;
}

int TestTimeCallBack(JEventLoop *el)
{
    TRACE("User time call back");
}

int main(int argc, char ** argv)
{
    jlog::OpenLog();
    jlog::SetLogLevel(jlog::LOGLEVEL::TRACE);
    INFO("jserver_test server start");
    JTcpServer svr("127.0.0.1", "9000", 4);
    svr.Init();
    svr.SetEventCallBack(TestEventCallBack);
    svr.SetTimeCallBack(TestTimeCallBack, 3000);
    svr.Run();
    return 0;
}