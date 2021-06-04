#include <iostream>
#include "router.cpp"
#include "switch.cpp"
using namespace std;

int main() {
    Switch mySwitch;
    Router myRouter;

    //SWITCH TEST:
    //------------------------------------------------------------

    //Each frame part used later has a limit of 16 bits, or 65,535
    //interface numbers is 0-3.
    Switch::EthernetFrame frame;

    //Show we can access the elements.
    u_int16_t* frameSourcePointer = frame.getSourceAddress();
    u_int16_t* frameDestinationPointer = frame.getDestinationAddress();

    //switch test.
    //This tests a frame not in table sending to frame not in table.
    frame.setSourceAddress(65535, 65535, 1);
    frame.setDestinationAddress(128, 3842, 9874);
    std::cout << mySwitch.setIncomingFrame(frame, 0);

    //This tests a frame IN TABLE sending to frame not in table
    frame.setSourceAddress(65535, 65535, 2);
    frame.setDestinationAddress(128, 3842, 9874);
    std::cout << mySwitch.setIncomingFrame(frame, 0);

    //This tests a frame being sent to a place with 2 frames.
    frame.setSourceAddress(65535, 65535, 3);
    frame.setDestinationAddress(65535, 65535, 1);
    std::cout << mySwitch.setIncomingFrame(frame, 1);

    //This tests a frame being sent to a place with 2 frames, choosing latter one.
    frame.setSourceAddress(65535, 65535, 4);
    frame.setDestinationAddress(65535, 65535, 2);
    std::cout << mySwitch.setIncomingFrame(frame, 2);

    //This tests sending to a normal interface, no stacking
    frame.setSourceAddress(65535, 65535, 5);
    frame.setDestinationAddress(65535, 65535, 4);
    std::cout << mySwitch.setIncomingFrame(frame, 3);
    std::cout << endl;

    //ROUTER TEST:
    //------------------------------------------------------------
    //For reference, the table is built like this:
//  Interface 0:  { 4294967295, 4294967295, 4294967295, 4294967295, 8 },      // Base condition.
//  Interface 1:  { 4294967295, 4294967295, 4294967295, 4294967295, 126 },    // Test choosing greater match over lesser.
//  Interface 2:  { 1, 1, 1, 1, 32 },                                         // Test different address.
//  Interface 3:  { 0, 0, 0, 0, 0 },                                          // Catch-all to test.
    Router::Ipv6Packet packet;

    //Show we can access the elements.
    u_int32_t* packetSourcePointer = packet.getSourceAddress();
    u_int32_t* packetDestinationPointer = packet.getDestinationAddress();

    //Test the catch-all.
    packet.setSourceAddress(1337, 1337, 1337, 1337);
    packet.setDestinationAddress(128, 128, 128, 128);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);

    //Test hop limit.
    packet.setSourceAddress(10987, 654, 321, 0);
    packet.setDestinationAddress(4294967295, 4294967295, 4294967295, 4294967295);
    packet.setHopLimit(0);
    std::cout << myRouter.incomingPacket(packet);

    //Test choosing the interface with the better match.
    packet.setSourceAddress(10987, 654, 321, 0);
    packet.setDestinationAddress(4294967295, 4294967295, 4294967295, 4294967295);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);

    //Test an address that is partly correct.
    packet.setSourceAddress(1234, 5678, 9101112, 13141516);
    packet.setDestinationAddress(1, 2, 2, 2);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);

    //Final test for normal functionality.
    packet.setSourceAddress(8462, 6842, 4268, 2684);
    packet.setDestinationAddress(1, 1, 1, 1);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);
}
