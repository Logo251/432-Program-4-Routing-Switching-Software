#include <cstdlib>

struct EthernetFrame {
    u_int preamble: (7*8);  // preamble is 7 bytes of data.
    u_int SFD: (1*8);       //SFD is 1 byte.
    u_int destinationAddress: (6*8);    //6 bytes.
    u_int sourceAddress: (6*8);         //6 bytes.
    u_int8_t length;
    char* payload[375]; //4 bytes x 375 = 1,500 bytes.
    u_int32_t CRC;
};

static class Switch {
public static void CreateFrame(u_int destinationAddress: (6*8), )
};