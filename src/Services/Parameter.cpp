#include "Services/Parameter.hpp"

Parameter::Parameter() {
	ptc = 0;
	pfc = 0;
	ptr = nullptr;
}

Parameter::Parameter(uint8_t newPtc, uint8_t newPfc, uint32_t initialValue, UpdatePtr newPtr) {
	ptc = newPtc;
	pfc = newPfc;
	ptr = newPtr;

	if (ptr != nullptr) {
		(*ptr)(&currentValue);  // call the update function for the initial value
	} else {
		currentValue = initialValue;
	}
}

void Parameter::setCurrentValue(ValueType newVal) {
	currentValue = newVal;
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

void Parameter::setFlag(etl::bitset<NUM_OF_FLAGS> flags) {
	this->flags |= flags;
}
