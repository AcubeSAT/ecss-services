#include <iostream>
#include <cstring>
#include "Message.hpp"

int main() {
    Message packet = Message(0, 0, Message::TC, 1);

    packet.appendString(5, "hello");
    packet.appendBits(15, 0x28a8);
    packet.appendBits(1, 1);
    packet.appendReal(5.7);

    std::cout << "Hello, World!" << std::endl;
    std::cout << std::hex << packet.data << std::endl; // packet data must be 'helloQQ'
//    std::cout << *(reinterpret_cast<float*>(packet.data + 7)) << std::endl;
    return 0;
}
