#include <ecss-services/inc/Logger.hpp>
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

void ParameterBase::setValueAsString(const String<MAX_STRING_LENGTH> &input) {
    memcpy(valuePtr, input.c_str(), sizeInBytes);
}
