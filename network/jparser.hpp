#ifndef J_PARSER_H
#define J_PARSER_H

#include <cstdint>
#include <string>
namespace jarvis
{

const uint16_t TYPE_UINT8 = 0x00;
const uint16_t TYPE_UINT16 = 0x01;
const uint16_t TYPE_UINT32 = 0x02;
const uint16_t TYPE_UINT64 = 0x03;
const uint16_t TYPE_INT8 = 0x04;
const uint16_t TYPE_INT16 = 0x05;
const uint16_t TYPE_INT32 = 0x06;
const uint16_t TYPE_INT64 = 0x07;
const uint16_t TYPE_DOUBLE = 0x08;
const uint16_t TYPE_STRING = 0X09;

// util functions
uint64_t ntohll(uint64_t num);
uint64_t htonll(uint64_t num);

/*
 * Serializable序列化的接口
 * 1.Serialize
 * 2.Deserialize
 */
class Serializable
{
public:
    virtual size_t Serialize(void * buf, size_t sz) = 0;
    virtual size_t Deserialize(const void * buf, size_t sz) = 0;
};

/*
 * JBaseTLV一个简单的Type + length + value结构
 */
class JBaseTLV: public Serializable
{
public:
    JBaseTLV(uint16_t t = 0, uint32_t l = 0, const char * b = NULL);
    JBaseTLV(const JBaseTLV & tlv);
    JBaseTLV& operator=(const JBaseTLV & tlv);
    ~JBaseTLV();

    size_t Serialize(void * buf, size_t sz); 
    size_t Deserialize(const void * buf, size_t sz);

    uint16_t GetType() const;
    uint32_t GetLen() const;
    char * GetData();
    const char * GetData() const;
protected:
    uint16_t type;
    uint32_t length;
    char * data;
    void Init(uint16_t t, uint32_t l, const char * b);
};

/*
 * JUint8TLV
 */
class JUint8TLV: public JBaseTLV
{
public:
    JUint8TLV(const uint8_t num);
    uint8_t GetVal() const;
};

/*
 * JUint16TLV
 */
class JUint16TLV: public JBaseTLV
{
public:
    JUint16TLV(const uint16_t num);
    uint16_t GetVal() const;
};

/*
 * JUint32TLV
 */
class JUint32TLV: public JBaseTLV
{
public:
    JUint32TLV(const uint32_t num);
    uint32_t GetVal() const;
};

/*
 * Juint64TLV
 */
class JUint64TLV: public JBaseTLV
{
public:
    JUint64TLV(const uint64_t num);
    uint64_t GetVal() const;
};

/*
 * JInt8TLV
 */
class JInt8TLV: public JBaseTLV
{
public:
    JInt8TLV(const int8_t num);
    int8_t GetVal() const;
};

/*
 * JInt16TLV
 */
class JInt16TLV: public JBaseTLV
{
public:
    JInt16TLV(const int16_t num);
    int16_t GetVal() const;
};

/*
 * JInt32TLV
 */
class JInt32TLV: public JBaseTLV
{
public:
    JInt32TLV(const int32_t num);
    int32_t GetVal() const;
};

/*
 * JInt64TLV
 */
class JInt64TLV: public JBaseTLV
{
public:
    JInt64TLV(const int64_t num);
    int64_t GetVal() const;
};

/*
 * JDoubleTLV
 */
class JDoubleTLV: public JBaseTLV
{
public:
    JDoubleTLV(const double num);
    double GetVal() const;
};

/*
 * JStringTLV
 */
class JStringTLV: public JBaseTLV
{
public:
    JStringTLV(const std::string & str);
    std::string GetVal() const;
};

}   // namespace jarvis

#endif
