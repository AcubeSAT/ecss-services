//
// Created by kyriakum on 3/1/2025.
//

#ifndef TESTMEMORY_HPP
#define TESTMEMORY_HPP

#include "Memory.hpp"
#include <etl/array.h>

class TestMemory : public Memory {
	public:
	    ReadData readData(std::uintptr_t address, std::uintptr_t offset) const override;

		bool writeData(std::uintptr_t address, std::uintptr_t offset, ReadData data) override;

	    TestMemory(LowerLimit lowerLimit, UpperLimit upperLimit) : Memory(lowerLimit, upperLimit) {}
	};
#endif //TESTMEMORY_HPP
