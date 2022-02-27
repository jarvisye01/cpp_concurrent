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
};

/*
 * JUint8TLV
 */
class JUint8TLV: public JBaseTLV
{
public:
    uint8_t GetVal() const;
};

/*
 * JUint16TLV
 */
class JUint16TLV: public JBaseTLV
{
public:
    uint16_t GetVal() const;
};

/*
 * JUint32TLV
 */
class JUint32TLV: public JBaseTLV
{
public:
    uint32_t GetVal() const;
};

/*
 * Juint64TLV
 */
class JUint64TLV: public JBaseTLV
{
public:
    uint64_t GetVal() const;
};

/*
 * JInt8TLV
 */
class JInt8TLV: public JBaseTLV
{
public:
    int8_t GetVal() const;
};

/*
 * JInt16TLV
 */
class JInt16TLV: public JBaseTLV
{
public:
    int16_t GetVal() const;
};

/*
 * JInt32TLV
 */
class JInt32TLV: public JBaseTLV
{
public:
    int32_t GetVal() const;
};

/*
 * JInt64TLV
 */
class JInt64TLV: public JBaseTLV
{
public:
    int64_t GetVal() const;
};

/*
 * JDoubleTLV
 */
class JDoubleTLV: public JBaseTLV
{
public:
    double GetVal() const;
};

/*
 * JStringTLV
 */
class JStringTLV: public JBaseTLV
{
public:
    std::string GetVal() const;
};

}   // namespace jarvis

#endif
