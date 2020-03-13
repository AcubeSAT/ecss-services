#include "Services/Parameter.hpp"

void ParameterBase::setFlags(const char* flags) {
	this->flags = Flags(flags);
}
