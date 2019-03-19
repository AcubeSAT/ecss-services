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
