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