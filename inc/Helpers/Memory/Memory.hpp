#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "Helpers/TypeDefinitions.hpp"

/**
 * OOP Implementation of handling memory reading & writing for executing ST[06] TCs.
 *
 * Each distinct subsystem shall implemented derived classes of Memory
 * that correspond to distinct memory types the subsystem accesses.
 */
class Memory {
public:

	/**
	 * Pure virtual readData method.
	 *
	 * As the name suggests, this method reads data and returns it.
	 * It's pure virtual, hence every derived class must override this method.
	 *
	 * @param address Base address to read from.
	 * @param offset Offset to base address to read from.
	 * @return Data it found at specified address in the form of ReadData.
	*/
	virtual ReadData readData(MemoryAddress address, MemoryAddress offset) const = 0;

	/**
	 * Pure virtual writeData method.
	 *
	 * As the name suggests, this method writes data.
	 * It's pure virtual, hence every derived class must override this method.
	 *
	 * @param address Base address to read from.
	 * @param offset Offset to base address to read from.
	 * @param data Data to write at specified address.
	 */
	virtual void writeData(MemoryAddress address, MemoryAddress offset, ReadData data) = 0;

	/**
	 * Virtual isValidAddress method.
	 *
	 * This method checks whether a specified address is within the address range(s) that maps to the specific memory.
	 *
	 * @param address Address to check for validity.
	 * @return true if address is valid else false.
	 */
	virtual bool isValidAddress(MemoryAddress address) const { return lowerLimit <= address && upperLimit >= address; }

	/**
	 * Constexpr constructor.
	 *
	 * Initializes the lower and upper limits of the address range of a memory instance.
	 */
	constexpr Memory(MemoryAddress lowerLimit, MemoryAddress upperLimit) : lowerLimit(lowerLimit), upperLimit(upperLimit) {}

	/**
	 * Default virtual destructor.
	 */
	virtual ~Memory() = default;

private:
	MemoryAddress lowerLimit;
	MemoryAddress upperLimit;
};

#endif //MEMORY_HPP
