#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <etl/array.h>
#include "Helpers/TypeDefinitions.hpp"

class Memory {
	public:
	    virtual ReadData readData(std::uintptr_t address, std::uintptr_t offset) const = 0;

		virtual bool writeData(std::uintptr_t address, std::uintptr_t offset,  ReadData data) = 0;

	   	bool isValidAddress(std::uintptr_t address) const { return lowerLimit <= address && upperLimit >= address; }

	    constexpr Memory(LowerLimit lowerLimit, UpperLimit upperLimit) : lowerLimit(lowerLimit), upperLimit(upperLimit) {}

	    virtual ~Memory() = default;

	    Memory(Memory const&) = default;
	    Memory& operator=(Memory const&) = default;

	    Memory(Memory&&) = delete;
	    Memory& operator=(Memory&&) = delete;

	private:
		LowerLimit lowerLimit;
	    UpperLimit upperLimit;

			};

#endif //MEMORY_HPP
