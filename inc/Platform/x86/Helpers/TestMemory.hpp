//
// Created by kyriakum on 3/1/2025.
//

#ifndef TESTMEMORY_HPP
#define TESTMEMORY_HPP

#include "IMemoryType.hpp"
#include <etl/array.h>

class TestMemory : public IMemoryType<TestMemory, 0, 0> {
public:
	etl::array<ReadData, ECSSMaxStringSize> readData(const std::size_t address, const MemoryDataLength& dataLength) const;

	bool writeData(const std::size_t address, const MemoryDataLength& dataLength, const etl::array<ReadData, const ECSSMaxStringSize>& data);

private:
	uint8_t readByte(const std::size_t address) const;

	bool writeByte(const std::size_t address);

	};
#endif //TESTMEMORY_HPP
