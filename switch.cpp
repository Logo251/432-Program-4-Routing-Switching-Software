#include <cstdlib>
#include <string>
#include <iomanip>
#include <vector>
using namespace std;

class Switch {

private:
    struct address {
        u_int16_t address[3];
    };
    //Local Variables
    vector<address> routingTable[4];

    //Local Functions

    //Checks table for the mac address, returns its interface or -1 for no entry.
    int checkTable(u_int16_t address[3]) {
        for(int i = 0; i < 4; i++) {
            if(checkEntry(address, i)) {
                return i;
            }
        }
        return -1;
    }

    //check if address is already bound to interface.
    //Returns true when it exists.
    bool checkEntry(u_int16_t address[3], int interface) {
        for(int i = 0; i < routingTable[interface].size(); i++) {
            if( routingTable[interface].at(i).address[0] == address[0] &&
                routingTable[interface].at(i).address[1] == address[1] &&
                routingTable[interface].at(i).address[2] == address[2]) {
                return true;
            }
        }
        return false;
    }

    //Adds the address and interface to table.
    void addToTable(u_int16_t address[3], int interface) {
        Switch::address temp;
        temp.address[0] = address[0];
        temp.address[1] = address[1];
        temp.address[2] = address[2];

        routingTable[interface].push_back(temp);
    }

    //Converts u_int16_t array to hex string.
    string int16ArrayToHexString(u_int16_t address[3]) {
        stringstream stream;
        string output;
        string temp;
        for(int i = 0; i < 3; i++) {
            stream.str(std::string());
            stream << std::hex << address[i];
            temp = stream.str();
            while(temp.length() < 4) {
                temp = "0" + temp;
            }
            //Formatting.
            temp.insert(2, ":");
            temp.append(":");
            output += temp;
        }
        output.erase(output.length()-1);
        return output;
    }

public:
    struct EthernetFrame {
        u_int64_t preamble: (7*8);  // preamble is 7 bytes of data.
        u_int SFD: (1*8);       //SFD is 1 byte.
        u_int16_t destinationAddress[3];
        u_int16_t sourceAddress[3];
        u_int16_t length; // 2 bytes
        char payload[375]; //4 bytes x 375 = 1,500 bytes.
        u_int32_t CRC;

        EthernetFrame() {
            preamble = 0;
            SFD = 171; //Equivalent to 10101011 in binary.
            length = 1500;
            for (int i = 0; i < 375; i++) { payload[i] = 'L'; }
            CRC = 2147483647;
        }

        u_int16_t* getSourceAddress() {return sourceAddress;}
        u_int16_t* getDestinationAddress() {return destinationAddress;}

        void setSourceAddress(u_int16_t first16bits, u_int16_t second16bits, u_int16_t third16bits) {
            sourceAddress[0] = first16bits;
            sourceAddress[1] = second16bits;
            sourceAddress[2] = third16bits;
        }

        void setDestinationAddress(u_int16_t first16bits, u_int16_t second16bits, u_int16_t third16bits) {
            destinationAddress[0] = first16bits;
            destinationAddress[1] = second16bits;
            destinationAddress[2] = third16bits;
        }
    };

    //Tells the switch there's an incoming Ethernet Frame on a specific interface.
    std::string setIncomingFrame(EthernetFrame incomingFrame, int interface) {
        //Bind in the routing table the interface number to the source address.
        //First, check if there's already a mac -> interface pairing.
        if (!checkEntry(incomingFrame.sourceAddress, interface)) {
            //In this case the address is not in table.
            addToTable(incomingFrame.sourceAddress, interface);
        }

        //Now see if we know where this packet should go.
        int outgoingInterface = checkTable(incomingFrame.destinationAddress);
        std::string returnStatement = "Frame from " + int16ArrayToHexString(incomingFrame.sourceAddress) + " to " +
        int16ArrayToHexString(incomingFrame.destinationAddress) + " was switched onto ";

        if (outgoingInterface == -1) {
            returnStatement += "all interfaces except " + to_string(interface) + '.';
        }
        else {
            returnStatement += "interface " + to_string(outgoingInterface) + '.';
        }
        return returnStatement + '\n';
    }
};
