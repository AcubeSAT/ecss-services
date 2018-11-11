#include <iostream>
#include <cstring>
#include "Message.h"

int main() {
    Message packet = Message();

    packet.appendString(5, "hello");
    packet.appendBits(15, 0x28a8);
    packet.appendBits(1, 1);
    packet.appendReal(5.7);

    std::cout << "Hello, World!" << std::endl;
    std::cout << std::hex << packet.data << std::endl; // packet data must be 'helloQQ'
    std::cout << *((float*) (packet.data + 7)) << std::endl;
    return 0;
}