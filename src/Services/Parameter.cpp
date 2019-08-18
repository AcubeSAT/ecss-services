#include "Services/Parameter.hpp"

Parameter::Parameter(uint8_t newPtc, uint8_t newPfc, ValueType initialValue, UpdatePtr newPtr) {
	ptc = newPtc;
	pfc = newPfc;
	ptr = newPtr;

	// see Parameter.hpp for explanation on flags
	// by default: no update priority, manual and automatic update available

	if (ptr != nullptr) {
		(*ptr)(&currentValue);  // call the update function for the initial value
	} else {
		currentValue = initialValue;
	}
}

void Parameter::setCurrentValue(ValueType newVal) {
	// set the value only if the parameter can be updated manually
	if (flags[2]) {
		currentValue = newVal;
	}
}

ValueType Parameter::getCurrentValue() {
	return currentValue;
}

uint8_t Parameter::getPTC() {
	return ptc;
}

uint8_t Parameter::getPFC() {
	return pfc;
}

void Parameter::setFlag(const char* flags) {
	this->flags = Flags(flags);
}
