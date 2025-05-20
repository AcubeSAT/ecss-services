#ifndef TESTMEMORY_HPP
#define TESTMEMORY_HPP

#include "Helpers/Memory/Memory.hpp"
#include <etl/span.h>

/**
* Acts as a type of memory for testing purposes
*/
class TestMemory : public Memory {
	public:
	    ReadData readData(MemoryAddress address, MemoryAddress offset) const override;

		void writeData(MemoryAddress address, MemoryAddress offset, ReadData data) override;

	    constexpr TestMemory() noexcept : Memory(0, DUMMY_SIZE), dummyArray{} {}

	    /**
		* Method to access the private dummy array safely
		*
		* @return etl::span "view" of dummy array
		*/
	    etl::span<uint8_t> getDummyArea() { return etl::span<uint8_t>(dummyArray); }

	private:
	    static constexpr std::size_t DUMMY_SIZE = 10000;
    	uint8_t dummyArray[DUMMY_SIZE] {0};

	};
#endif //TESTMEMORY_HPP
