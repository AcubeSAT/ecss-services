#include "Services/Parameter.hpp"

template <typename ValueType>
Parameter<ValueType>::Parameter(uint8_t newPtc, uint8_t newPfc, const ValueType& initialValue, void(*newPtr)(ValueType*)) {
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

template <typename ValueType>
void Parameter<ValueType>::setCurrentValue(const ValueType& newVal) {
	// set the value only if the parameter can be updated manually
	if (flags[1]) {
		currentValue = newVal;
	}
}

template <typename ValueType>
ValueType Parameter<ValueType>::getCurrentValue() {
	return currentValue;
}
template <typename ValueType>
uint8_t Parameter<ValueType>::getPTC() {
	return ptc;
}

template <typename ValueType>
uint8_t Parameter<ValueType>::getPFC() {
	return pfc;
}

template <typename ValueType>
void Parameter<ValueType>::setFlags(const char* flags) {
	this->flags = Flags(flags);
}

