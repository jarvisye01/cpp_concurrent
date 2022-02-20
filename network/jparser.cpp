#include <cctype>
#include <cstddef>
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include "arpa/inet.h"
#include "network/jparser.hpp"

namespace jarvis
{

// ==============JBaseTLV==============
JBaseTLV::JBaseTLV(uint16_t t, uint32_t l, const char * b): type(t), length(l)
{
    data = new char[length];
    memcpy(data, b, length);
}

JBaseTLV::JBaseTLV(const JBaseTLV & tlv): type(tlv.type), length(tlv.length)
{
    data = new char[length];
    memcpy(data, tlv.data, length);
}

JBaseTLV& JBaseTLV::operator=(const JBaseTLV & tlv)
{
    if (&tlv == this)
        return *this;

    type = tlv.type;
    length = tlv.length;
    if (data != NULL)
        delete [] data, data = NULL;
    data = new char[length];
    memcpy(data, tlv.data, length);
}

JBaseTLV::~JBaseTLV()
{
    if (data != NULL)
        delete [] data, data = NULL;
}

size_t JBaseTLV::Serialize(void * buf, size_t sz)
{
    int total = 2 + 4 + length;
    if (sz < total)
        return -1;
    uint16_t netType = -1;
    uint32_t netLen = -1;
    netType = htons(type);
    netLen = htonl(length);

    int idx = 0;
    memcpy(buf + idx, &netType, sizeof(uint16_t));
    idx += 2;
    memcpy(buf + idx, &netLen, sizeof(uint32_t));
    idx += 4;
    if (data != NULL)
        memcpy(buf + idx, data, length);
    return total;
}

size_t JBaseTLV::Deserialize(const void * buf, size_t sz)
{
    uint16_t netType = -1;
    uint32_t netLen = -1;

    int idx = 0;
    memcpy(&netType, buf + idx, sizeof(uint16_t));
    idx += 2;
    memcpy(&netLen, buf + idx, sizeof(uint32_t));
    idx += 4;

    type = ntohs(netType);
    length = ntohl(netLen);

    int total = 2 + 4 + length;
    if (sz < total)
        return -1;

    if (data != NULL)
        delete [] data, data = NULL;
    data = new char[length];
    memcpy(data, buf + idx, length);
    return total;
}

uint16_t JBaseTLV::GetType() const
{
    return type;
}

uint32_t JBaseTLV::GetLen() const
{
    return length;
}

char* JBaseTLV::GetData()
{
    return data;
}

const char* JBaseTLV::GetData() const
{
    return data;
}

}   // namespace jarvis
