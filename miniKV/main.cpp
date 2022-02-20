#include <cstddef>
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <vector>
#include "concurrent/jmap.hpp"
#include "network/jparser.hpp"
#include "network/jsocket.hpp"
#include "network/jeventloop.hpp"
#include "network/jserver.hpp"
#include "miniKV/minikv.hpp"

MinKv kvMap;

int Get(jarvis::jnet::JClientSocket * socket, jarvis::jnet::JEventLoop * looper, jarvis::JBaseTLV & tlv)
{
    jarvis::JBaseTLV keyTLV;
    keyTLV.Deserialize(tlv.GetData(), tlv.GetLen());
    std::string key(keyTLV.GetData(), keyTLV.GetLen());
    std::string val;

    kvMap.Get(key, val);
    jarvis::JBaseTLV retTLV(0, val.size(), val.c_str());
    std::vector<char> buf;
    buf.resize(2 + 4 + retTLV.GetLen());

    int ret = retTLV.Serialize(&buf[0], buf.size());
    if (ret < 0)
        return -1;

    socket->Write("yxj", 3);
    uint32_t netLen = htonl(buf.size());
    socket->Write(&netLen, sizeof(uint32_t));
    socket->Write(&buf[0], buf.size());
    printf("Get succ key %s val %s\n", key.c_str(), val.c_str());
    return 0;
}

int Set(jarvis::jnet::JClientSocket * socket, jarvis::jnet::JEventLoop * looper, jarvis::JBaseTLV & tlv)
{
    jarvis::JBaseTLV keyTLV, valTLV;
    int total = keyTLV.Deserialize(tlv.GetData(), tlv.GetLen());
    if (total < 0)
        return -1;

    std::string key(keyTLV.GetData(), keyTLV.GetLen());

    total = valTLV.Deserialize(tlv.GetData() + total, tlv.GetLen() - total);
    if (total < 0)
        return -1;

    std::string val(valTLV.GetData(), valTLV.GetLen());

    kvMap.Set(key, val);

    char retBuf[2 + 4];
    jarvis::JBaseTLV emptyTLV;
    emptyTLV.Serialize(retBuf, 6);

    socket->Write("yxj", 3);
    uint32_t netLen = htonl(6);
    socket->Write(&netLen, sizeof(uint32_t));
    socket->Write(retBuf, 6);
    printf("Set succ key %s val %s\n", key.c_str(), val.c_str());
    return 0;
}

int Dispatch(jarvis::jnet::JClientSocket * socket, jarvis::jnet::JEventLoop * looper)
{
    std::vector<char> buf;
    buf.resize(socket->GetRecvBufSize());
    int ret = socket->Read(&buf[0], socket->GetRecvBufSize()); 
    if (ret < 0)
        return -1;

    jarvis::JBaseTLV tlv;
    ret = tlv.Deserialize(&buf[0] + 7, buf.size() - 7);
    if (ret < 0)
        return -1;

    switch (tlv.GetType())
    {
        case 1:
          return Get(socket, looper, tlv);
        case 2:
          return Set(socket, looper, tlv);
        default:
          return -1;
    }

    return -1;
}

int main(int argc, char ** argv)
{
    int ret = 0;

    jarvis::jnet::JEasyTcpServer server(std::string("127.0.0.1"), 9000);
    int workSize = 8;
    jarvis::jnet::JEpoller epollers[8];
    jarvis::jnet::JEventLoop loopers[8];

    for (int i = 0; i < workSize; i++)
    {
        epollers[i].Create();
        loopers[i].SetPoller(&epollers[i]);
        loopers[i].SetEventCallBack(Dispatch);
        server.AddLooper(&loopers[i]);
    }

    ret = server.Init();
    ret = server.Run();
    return ret;
}
