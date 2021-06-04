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

    std::cout << "SWITCH TEST:\n";

    //Show we can access the elements.
    u_int16_t* frameSourcePointer = frame.getSourceAddress();
    u_int16_t* frameDestinationPointer = frame.getDestinationAddress();

    //switch test.
    //This tests a frame not in table sending to frame not in table.
    frame.setSourceAddress(0xffff, 0xffff, 0x1);
    frame.setDestinationAddress(0x80, 0xf02, 0x2692);
    std::cout << mySwitch.setIncomingFrame(frame, 0);

    //This tests a frame IN TABLE sending to frame not in table
    frame.setSourceAddress(0xffff, 0xffff, 0x2);
    frame.setDestinationAddress(0x80, 0xf02, 0x2692);
    std::cout << mySwitch.setIncomingFrame(frame, 0);

    //This tests a frame being sent to a place with 2 frames.
    frame.setSourceAddress(0xffff, 0xffff, 0x3);
    frame.setDestinationAddress(0xffff, 0xffff, 0x1);
    std::cout << mySwitch.setIncomingFrame(frame, 1);

    //This tests a frame being sent to a place with 2 frames, choosing latter one.
    frame.setSourceAddress(0xffff, 0xffff, 0x4);
    frame.setDestinationAddress(0xffff, 0xffff, 0x2);
    std::cout << mySwitch.setIncomingFrame(frame, 2);

    //This tests sending to a normal interface, no stacking
    frame.setSourceAddress(0xffff, 0xffff, 0x5);
    frame.setDestinationAddress(0xffff, 0xffff, 0x4);
    std::cout << mySwitch.setIncomingFrame(frame, 3);

    //For completeness send something to interface 1.
    frame.setSourceAddress(0xffff, 0xffff, 0x5);
    frame.setDestinationAddress(0xffff, 0xffff, 0x3);
    std::cout << mySwitch.setIncomingFrame(frame, 3);

    //For completeness send something to interface 3.
    frame.setSourceAddress(0xffff, 0xffff, 0x4);
    frame.setDestinationAddress(0xffff, 0xffff, 0x5);
    std::cout << mySwitch.setIncomingFrame(frame, 2);
    std::cout << endl;

    //ROUTER TEST:
    //------------------------------------------------------------
    //For reference, the table is built like this:
    //interface 0: { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 8 },      // Base condition.
    //interface 1: { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 126 },    // Test choosing greater match over lesser.
    //interface 2: { 0x1, 0x1, 0x1, 0x1, 32 },                                 // Test different address.
    //interface 3: { 0x0, 0x0, 0x0, 0x0, 0 },                                  // Catch-all to test.
    Router::Ipv6Packet packet;

    std::cout << "ROUTER TEST:\n";

    //Show we can access the elements.
    u_int32_t* packetSourcePointer = packet.getSourceAddress();
    u_int32_t* packetDestinationPointer = packet.getDestinationAddress();

    //Test the catch-all.
    packet.setSourceAddress(0x539, 0x539, 0x539, 0x539);
    packet.setDestinationAddress(0x80, 0x80, 0x80, 0x80);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);

    //Test hop limit.
    packet.setSourceAddress(0x2aeb, 0x28e, 0x141, 0x0);
    packet.setDestinationAddress(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
    packet.setHopLimit(0);
    std::cout << myRouter.incomingPacket(packet);

    //Test choosing the interface with the better match.
    packet.setSourceAddress(0x2aeb, 0x28e, 0x141, 0x0);
    packet.setDestinationAddress(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);

    //Test an address that is partly correct.
    packet.setSourceAddress(0x4d2, 0x162e, 0x8adf38, 0xc8860c);
    packet.setDestinationAddress(0x1, 0x2, 0x2, 0x2);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);

    //Final test for normal functionality.
    packet.setSourceAddress(0x210e, 0x1aba, 0x10ac, 0xa7c);
    packet.setDestinationAddress(0x1, 0x1, 0x1, 0x1);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);

    //For completeness, use interface 0.
    packet.setSourceAddress(0x210e, 0x1aba, 0x10ac, 0xa7c);
    packet.setDestinationAddress(0xFFAAAAAA, 0x0, 0x0, 0x0);
    packet.setHopLimit(8);
    std::cout << myRouter.incomingPacket(packet);
}
