//
// Created by kyriakum on 3/3/2025.
//

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <etl/array.h>
#include "TypeDefinitions.hpp"

class Memory {
	public:
	    virtual etl::array<ReadData, ECSSMaxStringSize> readData(std::size_t address, MemoryDataLength dataLength) const = 0;

		virtual bool writeData(std::size_t address, MemoryDataLength dataLength, const etl::array<ReadData, ECSSMaxStringSize>& data) = 0;

	    virtual bool isValidAddress(std::size_t address) const = 0;

			};

#endif //MEMORY_HPP
