#include "ServicePool.hpp"

ServicePool Services = ServicePool(); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void ServicePool::reset() {
	// Call the destructor
	this->~ServicePool();

	// Call the constructor
	// Note the usage of "placement new" that replaces the contents of the Services variable.
	// This is not dangerous usage, since all the memory that will be used for this has been
	// statically allocated from before.
	new (this) ServicePool();
}

uint16_t ServicePool::getAndUpdateMessageTypeCounter(uint8_t serviceType, uint8_t messageType) {
	uint16_t key = (serviceType << 8U) | messageType; // Create the key of the map
	return (messageTypeCounter[key])++;               // Fetch and increase the value
}

uint16_t ServicePool::getAndUpdatePacketSequenceCounter() {
	uint16_t value = packetSequenceCounter;

	// Increase the value
	if ((++packetSequenceCounter) >= (1U << 14U)) { // The value of the packet sequence counter is <= (2^14 - 1)
		packetSequenceCounter = 0;
	}

	return value;
}
