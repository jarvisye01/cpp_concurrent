#include <stdio.h>
#include <netdb.h>
#include "network/jserver.hpp"
#include "util/jlog.hpp"

using namespace std;
using namespace jarvis;

int main(int argc, char ** argv)
{
    jlog::OpenLog();
    jlog::SetLogLevel(jlog::LOGLEVEL::TRACE);
    INFO("jserver_test server start");
    JTcpServer svr("127.0.0.1", "9000", 4);
    svr.Init();
    svr.Run();
    return 0;
}