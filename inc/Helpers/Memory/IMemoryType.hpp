//
// Created by kyriakum on 3/1/2025.
//

#ifndef MEMORYACCESSHELPER_HPP
#define MEMORYACCESSHELPER_HPP

#include <etl/array.h>
#include "TypeDefinitions.hpp"

template <typename Memory, LowerLimit lowerLimit, UpperLimit upperLimit>
class IMemoryType {
	public:
		etl::array<ReadData, ECSSMaxStringSize> readData(const std::size_t address, const MemoryDataLength dataLength) const {
	    	return static_cast<Memory*>(this)->readData(address, dataLength);
		};

		bool writeData(const std::size_t address, const MemoryDataLength dataLength, const etl::array<ReadData, ECSSMaxStringSize>& data) {
			return static_cast<Memory*>(this)->writeData(address, dataLength, data);
	    }

	    constexpr LowerLimit getLowerLimit() const { return lowerLimit; }
	    constexpr UpperLimit getUpperLimit() const { return upperLimit; }

	    bool validateAddress(const std::size_t address) const {
		    return address >= lowerLimit && address <= upperLimit;
	    }
	private:

		uint8_t readByte(const std::size_t address) const {
		    return static_cast<Memory*>(this)->readByte(address);
		}

		bool writeByte(const std::size_t address) {
			return static_cast<Memory*>(this)->writeByte(address);
	    }
};

#endif //MEMORYACCESSHELPER_HPP
