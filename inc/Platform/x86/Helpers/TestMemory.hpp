#ifndef TESTMEMORY_HPP
#define TESTMEMORY_HPP

#include "Helpers/Memory/Memory.hpp"

class TestMemory : public Memory {
	public:
	    ReadData readData(std::uintptr_t address, std::uintptr_t offset) const override;

		bool writeData(std::uintptr_t address, std::uintptr_t offset, ReadData data) override;

	    constexpr TestMemory(LowerLimit lowerLimit, UpperLimit upperLimit) noexcept : Memory(lowerLimit, upperLimit) {}

	    ~TestMemory() override = default;
	};
#endif //TESTMEMORY_HPP
