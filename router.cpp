#include <cstdlib>

struct Ipv6Packet {
    u_int version: 4;       //Setting size of uint to 4 bits.
    u_int8_t priority;
    u_int flowLabel: 20;    //Setting size of uint to 4 bits.
    u_int16_t payloadLength;
    u_int8_t nextHeader;
    u_int8_t hopLimit;
    u_int32_t sourceAddress[4];
    u_int32_t destinationAddress[4];
    void* payload;

    u_int32_t* getSource() {return sourceAddress; } //Returns the first part of array as I understand,
                                                    //means that we can use this to get all the address parts.
    u_int32_t* getDestination() {return destinationAddress; }
    bool decreaseHopLimit() {
        hopLimit--;
        if(hopLimit > 0) {
            return true;
        }
        return false;
    }
};