#ifndef J_PARSER_H
#define J_PARSER_H

#include <cstdint>
namespace jarvis
{

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
private:
    uint16_t type;
    uint32_t length;
    char * data;
};

}   // namespace jarvis

#endif
