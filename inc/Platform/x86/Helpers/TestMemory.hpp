#ifndef TESTMEMORY_HPP
#define TESTMEMORY_HPP

#include "Helpers/Memory/Memory.hpp"
#include <etl/span.h>

class TestMemory : public Memory {
	public:
	    ReadData readData(std::uintptr_t address, std::uintptr_t offset) const override;

		void writeData(std::uintptr_t address, std::uintptr_t offset, ReadData data) override;

	    constexpr TestMemory() noexcept : Memory(0, DUMMY_SIZE), dummyArray{} {}

	    etl::span<uint8_t> getDummyArea() { return etl::span<uint8_t>(dummyArray); }

	private:
	    static constexpr std::size_t DUMMY_SIZE = 10000;
    	uint8_t dummyArray[DUMMY_SIZE] {0};

	};
#endif //TESTMEMORY_HPP
