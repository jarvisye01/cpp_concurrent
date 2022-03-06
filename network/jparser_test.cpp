#include <cstdint>
#include <stdio.h>
#include <string>
#include "network/jparser.hpp"

void printTLV(jarvis::JBaseTLV & tlv)
{
    char buf[1024];
    int len = tlv.Serialize(buf, 1024);
    for (int i = 0; i < len; i++)
    {
        printf("%d\t", buf[i]);
    }
    printf("\n");
}

int main(int argc, char ** argv)
{
    char buf[1024] = {0};

    uint8_t a = 1;
    jarvis::JUint8TLV a_tlv(a);
    a_tlv.Serialize(buf, 1024);
    printTLV(a_tlv);

    uint16_t b = 3;
    jarvis::JUint16TLV b_tlv(b);
    b_tlv.Serialize(buf, 1024);
    printTLV(b_tlv);

    uint32_t c = 1024;
    jarvis::JUint32TLV c_tlv(c);
    c_tlv.Serialize(buf, 1024);
    printTLV(c_tlv);
    
    uint64_t d = 2048;
    jarvis::JUint64TLV d_tlv(d);
    d_tlv.Serialize(buf, 1024);
    printTLV(d_tlv);

    int8_t e = 3;
    jarvis::JInt8TLV e_tlv(e);
    e_tlv.Serialize(buf, 1024);
    printTLV(e_tlv);

    int16_t f = 2334;
    jarvis::JInt16TLV f_tlv(f);
    f_tlv.Serialize(buf, 1024);
    printTLV(f_tlv);

    int32_t g = 32423;
    jarvis::JInt32TLV g_tlv(g);
    g_tlv.Serialize(buf, 1024);
    printTLV(g_tlv);

    int64_t h = 2342344;
    jarvis::JInt64TLV h_tlv(h);
    h_tlv.Serialize(buf, 1024);
    printTLV(h_tlv);

    double i = 3.14;
    jarvis::JDoubleTLV i_tlv(i);
    i_tlv.Serialize(buf, 1024);
    printTLV(i_tlv);

    std::string j("hello, world!");
    jarvis::JStringTLV j_tlv(j);
    j_tlv.Serialize(buf, 1024);
    printTLV(j_tlv);

    printf("a: %d\n", a_tlv.GetVal());
    printf("b: %d\n", b_tlv.GetVal());
    printf("c: %u\n", c_tlv.GetVal());
    printf("d: %llu\n", d_tlv.GetVal());

    printf("e: %d\n", e_tlv.GetVal());
    printf("f: %d\n", f_tlv.GetVal());
    printf("g: %u\n", g_tlv.GetVal());
    printf("h: %llu\n", h_tlv.GetVal());

    printf("i: %lf\n", i_tlv.GetVal());
    printf("j: %s\n", j_tlv.GetVal().c_str());

    return 0;
}
