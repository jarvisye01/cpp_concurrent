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
// util functions
uint64_t ntohll(uint64_t num)
{
    // 主机是大端序 big endian
    if (__BYTE_ORDER == __BIG_ENDIAN)
        return num;

    // 主机是小端序 little endian
    uint64_t high = ntohl((num << 32) >> 32);
    uint64_t low = ntohl(num >> 32);
    return (high << 32) | low;
}

uint64_t htonll(uint64_t num)
{
    // host is big endian
    if (__BYTE_ORDER == __BIG_ENDIAN)
        return num;

    // host is little endian
    uint64_t high = htonl((num << 32) >> 32);
    uint64_t low = htonl(num >> 32);
    return (high << 32) | low;
}

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

void JBaseTLV::Init(uint16_t t, uint32_t l, const char * b)
{
    type = t;
    length = l;
    if (data)
        delete [] data, data = NULL;
    data = new char[length];
    memcpy(data, b, length);
}

// =============JUint8==============
JUint8TLV::JUint8TLV(const uint8_t num): JBaseTLV(TYPE_UINT8, sizeof(uint8_t), (const char*)(&num))
{
}

uint8_t JUint8TLV::GetVal() const
{
    if (type != TYPE_UINT8)
        return -1;
    uint8_t tmp = *((uint8_t*)(data));
    return tmp;
}

// ============JUint16==============
JUint16TLV::JUint16TLV(const uint16_t num)
{
    uint16_t tnum = htons(num);
    Init(TYPE_UINT16, sizeof(uint16_t), (const char*)(&num));
}

uint16_t JUint16TLV::GetVal() const
{
    if (type != TYPE_UINT16)
        return -1;
    uint16_t tmp = *((uint16_t*)(data));
    return ntohs(tmp);
}

// ============JUint32==============
JUint32TLV::JUint32TLV(const uint32_t num)
{
    uint32_t tnum = htonl(num);
    Init(TYPE_UINT32, sizeof(uint32_t), (const char*)(&tnum));
}

uint32_t JUint32TLV::GetVal() const
{
    if (type != TYPE_UINT32)
        return -1;
    uint32_t tmp = *((uint32_t*)(data));
    return ntohl(tmp);
}

// ============JUint64=============
JUint64TLV::JUint64TLV(const uint64_t num)
{
    uint64_t tnum = htonll(num);
    Init(TYPE_UINT64, sizeof(uint64_t), (const char*)(&tnum));
}

uint64_t JUint64TLV::GetVal() const
{
    if (type != TYPE_UINT64)
        return -1;

    uint64_t tmp = *((uint64_t*)(data));
    return ntohll(tmp);
}

// ============JInt8=============
JInt8TLV::JInt8TLV(const int8_t num): JBaseTLV(TYPE_INT8, sizeof(int8_t), (const char*)(&num))
{
}

int8_t JInt8TLV::GetVal() const
{
    if (type != TYPE_INT8)
        return -1;
    int8_t tmp = *((int8_t*)(data));
    return tmp;
}

// ============JInt16=============
JInt16TLV::JInt16TLV(const int16_t num)
{
    uint16_t tnum = htons(num);
    Init(TYPE_INT16, sizeof(int16_t), (const char*)(&tnum));
}

int16_t JInt16TLV::GetVal() const
{
    if (type != TYPE_INT16)
        return -1;
    uint16_t tmp = *((uint16_t*)(data));
    return static_cast<int16_t>(ntohs(tmp));
}

// ============JInt32=============
JInt32TLV::JInt32TLV(const int32_t num)
{
    uint32_t tnum = htonl(num);
    Init(TYPE_INT32, sizeof(int32_t), (const char*)(&tnum));
}

int32_t JInt32TLV::GetVal() const
{
    if (type != TYPE_INT32)
        return -1;
    uint32_t tmp = *((uint32_t*)(data));
    return static_cast<int32_t>(ntohl(tmp));
}

// ============JInt64=============
JInt64TLV::JInt64TLV(const int64_t num)
{
    uint64_t tnum = htonll(num);
    Init(TYPE_INT64, sizeof(int64_t), (const char*)(&tnum));
}

int64_t JInt64TLV::GetVal() const
{
    if (type != TYPE_INT64)
        return -1;

    uint64_t tmp = *((uint64_t*)(data));
    return static_cast<int64_t>(ntohll(tmp));
}

// =============JDoubleTLV===============
JDoubleTLV::JDoubleTLV(const double num)
{
    uint64_t tmp;
    memcpy(&tmp, &num, sizeof(uint64_t));
    tmp = htonll(tmp);
    Init(TYPE_DOUBLE, sizeof(double), (const char*)(&tmp));
}

double JDoubleTLV::GetVal() const
{
    if (type != TYPE_DOUBLE)
        return -1;

    uint64_t tmp = *((uint64_t*)(data));
    tmp = ntohll(tmp);

    double ret;
    memcpy(&ret, &tmp, sizeof(uint64_t));
    return ret;
}

// ================JStringTLV================
JStringTLV::JStringTLV(const std::string & str): JBaseTLV(TYPE_STRING, str.size(), str.c_str())
{
}

std::string JStringTLV::GetVal() const
{
    if (type != TYPE_STRING)
        return std::string("");
    return std::string(data, length);
}

}   // namespace jarvis
