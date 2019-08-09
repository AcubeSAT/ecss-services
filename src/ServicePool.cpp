#include "ServicePool.hpp"

ServicePool Services = ServicePool();

void ServicePool::reset() {
	// Call the destructor
	this->~ServicePool();

	// Call the constructor
	// Note the usage of "placement new" that replaces the contents of the Services variable.
	// This is not dangerous usage, since all the memory that will be used for this has been
	// statically allocated from before.
	new (this) ServicePool();
}

uint16_t ServicePool::getMessageTypeCounter(uint8_t serviceType, uint8_t messageType) {
	uint16_t key = (serviceType << 8u) | messageType; // Create the key of the map
	return (messageTypeCounter[key])++; // Fetch and increase the value
}
