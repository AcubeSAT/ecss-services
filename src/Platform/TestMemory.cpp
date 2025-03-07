//
// Created by kyriakum on 8/3/2025.
//

#include <inc/Platform/x86/Helpers/TestMemory.hpp>
#include "ECSS_Definitions.hpp"
#include "TypeDefinitions.hpp"
#include "MemoryManagementService.hpp"
#include "Message.hpp"

bool TestMemory::writeData(Message& request, const etl::array<ReadData, ECSSMaxStringSize>& data)
{
	uint16_t const iterationCount = request.readUint16();

	for(uint16_t i = 0; i < iterationCount; ++i) {
		const StartAddress startAddress = request.read<StartAddress>();
		const MemoryDataLength dataLength = request.readOctetString(readData.data()); // NOLINT(cppcoreguidelines-init-variables)
		const MemoryManagementChecksum checksum = request.readBits(BitsInMemoryManagementChecksum);

		// VALIDATE DATA

		// VALIDATE ADDRESS

		// WRITE DATA

		// READ DATA FOR VALIDATION

		// CHECK CRC
	}

	// RETURN TYPE WILL CHANGE

	return true;
}
