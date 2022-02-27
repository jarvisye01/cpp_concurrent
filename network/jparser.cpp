#include <cctype>
#include <cstddef>
#include <cstdint>
#include <endian.h>
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

// =============JUint8==============
uint8_t JUint8TLV::GetVal() const
{
    if (type != TYPE_UINT8)
        return -1;
    uint8_t tmp = *((uint8_t*)(data));
    return tmp;
}

// ============JUint16==============
uint16_t JUint16TLV::GetVal() const
{
    if (type != TYPE_UINT16)
        return -1;
    uint16_t tmp = *((uint16_t*)(data));
    return ntohs(tmp);
}

// ============JUint32==============
uint32_t JUint32TLV::GetVal() const
{
    if (type != TYPE_UINT32)
        return -1;
    uint32_t tmp = *((uint32_t*)(data));
    return ntohl(tmp);
}

// ============JUint64=============
uint64_t JUint64TLV::GetVal() const
{
    if (type != TYPE_UINT64)
        return -1;

    uint64_t tmp = *((uint64_t*)(data));
    // 主机也是big_endian，直接返回
    if (__BYTE_ORDER == __BIG_ENDIAN)
        return tmp;
    
    // 主机是little_endian
    uint64_t high = ntohl((tmp << 32) >> 32) << 32;
    uint64_t low = ntohl(tmp >> 32);
    return high | low;
}

// ============JInt8=============
int8_t JInt8TLV::GetVal() const
{
    if (type != TYPE_INT8)
        return -1;
    int8_t tmp = *((int8_t*)(data));
    return tmp;
}

// ============JInt16=============
int16_t JInt16TLV::GetVal() const
{
    if (type != TYPE_INT16)
        return -1;
    uint16_t tmp = *((uint16_t*)(data));
    return static_cast<int16_t>(ntohs(tmp));
}

// ============JInt32=============
int32_t JInt32TLV::GetVal() const
{
    if (type != TYPE_INT32)
        return -1;
    uint32_t tmp = *((uint32_t*)(data));
    return static_cast<int32_t>(ntohl(tmp));
}

// ============JInt64=============
int64_t JInt64TLV::GetVal() const
{
    if (type != TYPE_INT64)
        return -1;

    uint64_t tmp = *((uint64_t*)(data));
    if (__BYTE_ORDER == __BIG_ENDIAN)
        return static_cast<int64_t>(tmp);

    uint64_t high = ntohl((tmp << 32) >> 32) << 32;
    uint64_t low = ntohl(tmp >> 32);
    return static_cast<int64_t>(high | low);
}

// =============JDoubleTLV===============
double JDoubleTLV::GetVal() const
{
    if (type != TYPE_DOUBLE)
        return -1;

    uint64_t tmp = *((uint64_t*)(data));
    if (__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        uint64_t high = ntohl((tmp << 32) >> 32) << 32;
        uint64_t low = ntohl(tmp >> 32);
        tmp = high | low;
    }

    double ret;
    memcpy(&ret, &tmp, sizeof(double));
    return ret;
}

// ================JStringTLV================
std::string JStringTLV::GetVal() const
{
    if (type != TYPE_STRING)
        return std::string("");
    return std::string(data, length);
}

}   // namespace jarvis
