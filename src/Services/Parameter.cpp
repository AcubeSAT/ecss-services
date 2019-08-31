#include "Services/Parameter.hpp"

uint8_t ParameterBase::getPTC() {
	return ptc;
}

uint8_t ParameterBase::getPFC() {
	return pfc;
}

void ParameterBase::setFlags(const char* flags) {
	this->flags = Flags(flags);
}