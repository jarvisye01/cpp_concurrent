#include <cstdint>
#include <stdio.h>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "network/jsocket.hpp"
#include "network/jparser.hpp"

int Send(jarvis::jnet::JClientSocket & client, jarvis::JBaseTLV & tlv)
{
    std::vector<char> buf;
    buf.resize(2 + 4 + tlv.GetLen());
    tlv.Serialize(&buf[0], buf.size());

    int netLen = -1;
    client.Write("yxj", 3);
    netLen = htonl(buf.size());
    client.Write(&netLen, sizeof(uint32_t));
    client.Write(&buf[0], buf.size());

    return jarvis::jnet::JEasyDataHandler().HandleOut(&client);
}

int Recv(jarvis::jnet::JClientSocket & client, jarvis::JBaseTLV & tlv)
{
    int ret = 0;
    ret = jarvis::jnet::JEasyDataHandler().HandleIn(&client);
    if (ret != 0)
        return -1;

    std::vector<char> buf;
    buf.resize(client.GetRecvBufSize());
    client.Read(&buf[0], buf.size());

    ret = tlv.Deserialize(&buf[7], buf.size() - 7);
    return ret > 0 ? 0 : -1;
}

int Set(jarvis::jnet::JClientSocket & client, const std::string & key, const std::string & val)
{
    jarvis::JBaseTLV keyTLV(0, key.size(), key.c_str());
    jarvis::JBaseTLV valTLV(0, val.size(), val.c_str());

    std::vector<char> buf;
    buf.resize(4 + 8 + keyTLV.GetLen() + valTLV.GetLen());
    int ret = keyTLV.Serialize(&buf[0], buf.size());
    if (ret < 0)
        return -1;

    ret = valTLV.Serialize(&buf[ret], buf.size() - ret);
    if (ret < 0)
        return -1;

    jarvis::JBaseTLV sendTLV(2, buf.size(), &buf[0]);
    ret = Send(client, sendTLV);
    if (ret < 0)
        return -1;

    jarvis::JBaseTLV recvTLV;
    return Recv(client, recvTLV);
}

int Get(jarvis::jnet::JClientSocket & client, const std::string & key, std::string & val)
{
    jarvis::JBaseTLV keyTLV(0, key.size(), key.c_str());
    std::vector<char> buf;
    buf.resize(2 + 4 + keyTLV.GetLen());
    int ret = keyTLV.Serialize(&buf[0], buf.size());
    if (ret < 0)
        return -1;
    jarvis::JBaseTLV sendTLV(1, buf.size(), &buf[0]);
    ret = Send(client, sendTLV);
    if (ret < 0)
        return -1;
    jarvis::JBaseTLV recvTLV;
    ret = Recv(client, recvTLV);
    if (ret < 0)
        return -1;
    val = std::string(recvTLV.GetData(), recvTLV.GetLen());
    return 0;
}

int main(int argc, char ** argv)
{
    if (argc < 2)
        return -1;

    int ret = -1;
    jarvis::jnet::JNetAddress addr(std::string("127.0.0.1"), 9000); 
    jarvis::jnet::JClientSocket client(AF_INET, SOCK_STREAM, 0);
    ret = client.Connect(addr);
    if (ret < 0)
    {
        printf("connnect error\n");
        return -1;
    }

    if (std::string(argv[1]) == "set")
    {
        if (argc < 4)
        {
            printf("error param\n");
            return -1;
        }
        ret = Set(client, std::string(argv[2]), std::string(argv[3]));
        if (ret == 0)
        {
            printf("set succ %s => %s\n", argv[2], argv[3]);
        }
        else
        {
            printf("set error\n");
            return -1;
        }
    }
    else if (std::string(argv[1]) == "get")
    {
        if (argc < 3)
        {
            printf("error param\n");
            return -1;
        }
        std::string val;
        ret = Get(client, std::string(argv[2]), val);
        if (ret == 0)
        {
            printf("get succ %s => %s\n", argv[2], val.c_str());
        }
        else
        {
            printf("get error\n");
            return -1;
        }
    }
    return 0;
}
