#include "Services/Parameter.hpp"

template <typename ValueType>
Parameter<ValueType>::Parameter(uint8_t newPtc, uint8_t newPfc, ValueType initialValue, void (*
newPtr)(ValueType*)) {
	ptc = newPtc;
	pfc = newPfc;
	ptr = newPtr;
	sizeInBytes = sizeof(initialValue);
	valuePtr = static_cast<void *>(&currentValue);
	// see Parameter.hpp for explanation on flags
	// by default: no update priority, manual and automatic update available

	if (ptr != nullptr) {
		(*ptr)(&currentValue);  // call the update function for the initial value
	} else {
		currentValue = initialValue;
	}
}

template <typename ValueType>
void ParameterBase::setCurrentValue(ValueType newVal) {
	// set the value only if the parameter can be updated manually
	if (flags[1]) {
		*reinterpret_cast<ValueType>(valuePtr) = newVal;
	}
}

template <typename ValueType>
String<MAX_STRING_LENGTH> Parameter<ValueType>::getValueAsString() {
	String<MAX_STRING_LENGTH> contents(reinterpret_cast<uint8_t*>(&currentValue), sizeInBytes);
	return contents;
}

uint8_t ParameterBase::getPTC() {
	return ptc;
}

uint8_t ParameterBase::getPFC() {
	return pfc;
}

void ParameterBase::setFlags(const char* flags) {
	this->flags = Flags(flags);
}