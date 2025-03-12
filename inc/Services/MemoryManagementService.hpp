#ifndef ECSS_SERVICES_MEMMANGSERVICE_HPP
#define ECSS_SERVICES_MEMMANGSERVICE_HPP

#include <memory>
#include "ErrorHandler.hpp"
#include "Helpers/CRCHelper.hpp"
#include "Helpers/Memory/MemoryAddressProvider.hpp"
#include "Helpers/TypeDefinitions.hpp"
#include "Service.hpp"

/**
 * Number of Bits in Memory Management Checksum
 * TODO move this somewhere else
 */
inline constexpr uint32_t BitsInMemoryManagementChecksum = 8 * sizeof(MemoryManagementChecksum);

/**
 * @ingroup Services
 */
class MemoryManagementService : public Service {
public:
	inline static constexpr ServiceTypeNum ServiceType = 6;

	enum MessageType : uint8_t {
		LoadRawMemoryDataAreas = 2,
		DumpRawMemoryData = 5,
		DumpRawMemoryDataReport = 6,
		CheckRawMemoryData = 9,
		CheckRawMemoryDataReport = 10,
	};

	MemoryManagementService();

	/**
	 * Raw data memory management subservice class
	 *
	 * @details A class defining the raw data memory management subservice functions.
	 * 			As per the ECSS manual, each memory service has to have at most one raw memory
	 * 			data management subservice
	 */
	class RawDataMemoryManagement {
	private:
		MemoryManagementService& mainService; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members) // Used to access main class's members

	public:
		explicit RawDataMemoryManagement(MemoryManagementService& parent);

		/**
		 * TC[6,5] read raw memory values
		 *
		 * @details This function reads the raw data from the RAM memory and
		 * 			triggers a TM[6,6] report
		 * @param request Provide the received message as a parameter
		 * @todo (#221) In later embedded version, implement error checking for address validity for
		 * 		 different memory types
		 * @todo (#222) Only allow aligned memory address to be start addresses
		 */
		void dumpRawData(Message& request);

		/**
		 * TC[6,9] check raw memory data
		 *
		 * @details This function reads the raw data from the specified memory and
		 * 			triggers a TM[6,10] report
		 * @param request Provide the received message as a parameter
		 * @todo (#221) In later embedded version, implement error checking for address validity for
		 * 		 different memory types
		 * @todo (#222) Only allow aligned memory address to be start addresses
		 */
		void checkRawData(Message& request);
	} rawDataMemorySubservice;

	/**
	 * TC[6,2] load raw values to memory
	 *
	 * @details This function loads new values to memory data areas
	 * 			specified in the request
	 * @param request Provide the received message as a parameter
	 * @todo (#222) Only allow aligned memory address to be start addresses
	 */
	static void loadRawData(Message& request);

	/**
	 * It is responsible to call the suitable function that executes a telecommand packet. The source of that packet
	 * is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);

private:
	/**
	 * Get Memory from ID
	 *
	 * @param memId The memory ID to get
	 */
	static Memory* getMemoryFromId(MemoryId memId);

	/**
	 * Validate the data according to checksum calculation
	 */
	static bool dataValidator(const uint8_t* data, MemoryManagementChecksum checksum, MemoryDataLength length);
};

#endif // ECSS_SERVICES_MEMMANGSERVICE_HPP
