//
// Created by kyriakum on 3/1/2025.
//

#ifndef TESTMEMORY_HPP
#define TESTMEMORY_HPP

#include "Memory.hpp"
#include <etl/array.h>

class TestMemory : public Memory {
	public:
	    etl::array<ReadData, ECSSMaxStringSize> readData(std::size_t address, MemoryDataLength dataLength) const override;

		bool writeData(Message& request, const etl::array<ReadData, ECSSMaxStringSize>& data) override;

	    TestMemory(LowerLimit lowerLimit, UpperLimit upperLimit) : Memory(lowerLimit, upperLimit) {}
	};
#endif //TESTMEMORY_HPP
