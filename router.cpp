#include <cstdlib>
#include <string>
#include <iomanip>
using namespace std;

class Router {
public:
    //Local variables
    //5th spot is how many bits we deal with.
    //each entry is 32 bits, which means max size is 4,294,967,295
    u_int32_t table[4][5] =
            {
                { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 8 },      // Base condition.
                { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 126 },    // Test choosing greater match over lesser.
                { 0x1, 0x1, 0x1, 0x1, 32 },                                 // Test different address.
                { 0x0, 0x0, 0x0, 0x0, 0 },                                  // Catch-all to test.
            };

    struct Ipv6Packet {
        u_int version: 4;       //Setting size of uint to 4 bits.
        u_int8_t priority;
        u_int flowLabel: 20;    //Setting size of uint to 4 bits.
        u_int16_t payloadLength;
        u_int8_t nextHeader;
        u_int8_t hopLimit;
        u_int32_t sourceAddress[4];
        u_int32_t destinationAddress[4];
        void *payload;

        u_int32_t* getSourceAddress() {return sourceAddress;}
        u_int32_t* getDestinationAddress() {return destinationAddress;}

        void setSourceAddress(u_int32_t first32bits, u_int32_t second32bits, u_int32_t third32bits, u_int32_t fourth32bits) {
            sourceAddress[0] = first32bits;
            sourceAddress[1] = second32bits;
            sourceAddress[2] = third32bits;
            sourceAddress[3] = fourth32bits;
        }

        void setDestinationAddress(u_int32_t first32bits, u_int32_t second32bits, u_int32_t third32bits, u_int32_t fourth32bits) {
            destinationAddress[0] = first32bits;
            destinationAddress[1] = second32bits;
            destinationAddress[2] = third32bits;
            destinationAddress[3] = fourth32bits;
        }

        void setHopLimit(u_int8_t input) {
            hopLimit = input;
        }

        bool decreaseHopLimit() {
            //-1 comes around as 255, so if we -1 and it gets bigger it's bad.
            u_int8_t currentHop = hopLimit;
            hopLimit--;
            if (hopLimit < currentHop) {
                return true;
            }
            return false;
        }
    };

    //Determines which interface the packet goes out.
    std::string incomingPacket(Ipv6Packet packet) {
        int numberOfBits;
        int bestTableMatch = -1;
        int bestTableMatchBits = -1;
        u_int32_t shiftedDestination, shiftedTable;
        //First decrease packet hop since we're moving it.
        //If this returns false it means we don't move it on, we're done.
        if (!packet.decreaseHopLimit()) {
            return "Packet from " + int32ArrayToHexString(packet.sourceAddress) + " was Dropped due to Hop Limit expiration\n";
        }

        //Figure out what interface it goes out.
        //We take best complete match.
        for(int i = 0; i < 4; i++) {
            //Get the number of bits.
            numberOfBits = table[i][4];

            for (int j = 0; j < 4; j++) {
                if(numberOfBits > 32) {
                    if(packet.destinationAddress[j] == table[i][j]) {
                        numberOfBits -= 32;
                    }
                    else {
                        numberOfBits = -1; //To show it was a bad entry.
                        break; //Uses a break since we're done with this entry, it doesn't exactly match.
                    }
                }
                else { //We need to match less than the entire 32 bits of an int.
                    //Shift rights are rotating, shifting 32 puts it back where it is. Therefore, we need an exception.
                    //If it is 0 no bits need to match.
                    if(numberOfBits != 0) {
                        shiftedTable = table[i][j];
                        shiftedDestination = packet.destinationAddress[j];
                        shiftedTable = shiftedTable >> (32 - numberOfBits);
                        shiftedDestination = shiftedDestination >> (32 - numberOfBits);
                        numberOfBits = 0;
                        if(shiftedDestination != shiftedTable) {
                            numberOfBits = -1; //To show it was a bad entry.
                            break; //Uses a break since we're done with this entry, it doesn't exactly match.
                        }
                    }
                }
                if(numberOfBits == 0) { //Complete match
                    if((int)table[i][4] > bestTableMatchBits) { //Cast as int so the comparison works.
                        bestTableMatchBits = table[i][4];
                        bestTableMatch = i;
                    }
                    break;  //This break is for efficiency, so we don't continue through the entry we've confirmed
                            //because the bits required is 0.
                }
            }

        }
        //We've gone through all the table entries by now.
        if(bestTableMatch == -1) {
            return "No possible output.\n";
        }
        else {
            return "Packet from " + int32ArrayToHexString(packet.sourceAddress) + " to "
                + int32ArrayToHexString(packet.destinationAddress) +" was routed on interface " +
                to_string(bestTableMatch) + '\n';
        }
    }

    std::string int32ArrayToHexString(u_int32_t address[4]) {
        stringstream stream;
        string output;
        string temp;
        for(int i = 0; i < 4; i++) {
            stream.str(std::string());
            stream << std::hex << address[i];
            temp = stream.str();
            while(temp.length() < 8) {
                temp = "0" + temp;
            }
            //Formatting.
            temp.insert(4, ":");
            temp.append(":");
            output += temp;
        }
        output.erase(output.length()-1);
        return output;
    }


};