#ifndef TESTMEMORY_HPP
#define TESTMEMORY_HPP

#include "Helpers/Memory/Memory.hpp"

class TestMemory : public Memory {
	public:
	    ReadData readData(std::uintptr_t address, std::uintptr_t offset) const override;

		void writeData(std::uintptr_t address, std::uintptr_t offset, ReadData data) override;

	    constexpr TestMemory(AddressLimit lowerLimit, AddressLimit upperLimit) noexcept : Memory(lowerLimit, upperLimit) {}

	};
#endif //TESTMEMORY_HPP
