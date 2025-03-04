//
// Created by kyriakum on 3/1/2025.
//

#ifndef TESTMEMORY_HPP
#define TESTMEMORY_HPP

#include "Memory.hpp"
#include <etl/array.h>

template<LowerLimit lowerLimit, UpperLimit upperLimit>
class TestMemory : public Memory {
	public:
	    etl::array<ReadData, ECSSMaxStringSize> readData(std::size_t address, MemoryDataLength dataLength) const override {etl::array<ReadData, ECSSMaxStringSize> result;
        return result;}

		bool writeData(std::size_t address, MemoryDataLength dataLength, const etl::array<ReadData, ECSSMaxStringSize>& data) override { return true;}

	    bool isValidAddress(std::size_t address) const override { return lowerLimit <= address && upperLimit >= address; }

	};
#endif //TESTMEMORY_HPP
