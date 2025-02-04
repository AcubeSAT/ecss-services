#ifndef ECSS_SERVICES_MEMMANGSERVICE_HPP
#define ECSS_SERVICES_MEMMANGSERVICE_HPP

#include <memory>
#include "ErrorHandler.hpp"
#include "Helpers/CRCHelper.hpp"
#include "MemoryAddressLimits.hpp"
#include "Service.hpp"
#include "etl/unordered_map.h"
#include "etl/unordered_set.h"

/**
 * Number of Bits in Memory Management Checksum
 */
inline constexpr uint32_t BitsInMemoryManagementChecksum = 8 * sizeof(MemoryManagementChecksum);

/**
 * Maximum number of entries in Memory Limits Map
 */
inline constexpr uint32_t MaxMemoryLimitsMapSize = 8;

/**
 * Maximum number of entries in Valid Memory IDs set
 */
inline constexpr uint32_t MaxValidMemoryIdsSize = 8;

/**
 * @ingroup Services
 */
class MemoryManagementService : public Service {
public:
	inline static constexpr ServiceTypeNum ServiceType = 6;

	enum MessageType : uint8_t {
		LoadObjectMemoryData = 1,
		LoadRawMemoryDataAreas = 2,
		DumpObjectMemoryData = 3,
		DumpedObjectMemoryDataReport = 4,
		DumpRawMemoryData = 5,
		DumpRawMemoryDataReport = 6,
		CheckRawMemoryData = 9,
		CheckRawMemoryDataReport = 10,
	};

	/**
	 * The IDs of memories managed by the current service
	 */
	enum MemoryID {
		DTCMRAM = 0,
		RAM_D1,
		RAM_D2,
		RAM_D3,
		ITCMRAM,
		FLASH_MEMORY,
		EXTERNAL,
	};

	MemoryManagementService();

	/**
	 * Raw data memory management subservice class
	 *
	 * @details A class defining the raw data memory management subservice functions.
	 * 			As per the ECSS manual, each memory service has to have at most one raw memory
	 * 			data management subservice
	 */
	class RawDataMemoryManagementSubService {
	private:
		MemoryManagementService& mainService; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members) // Used to access main class's members

	public:
		explicit RawDataMemoryManagementSubService(MemoryManagementService& parent);

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
	 * Structured data memory management subservice class. This class offers the capability of handling structured data
	 * in a memory. For example, it can handle the transfer of a stored file from a Memory to the Ground Station.
	 * @note The base plus offset scheme, as defined by 6.6.4.3 is the repository path + file name
	 * @note We assume that only one memory is managed by the current subservice. That means that we don't read for memory ID.
	 */
	class StructuredDataMemoryManagementSubService {
		private:
		MemoryManagementService& mainService;

	public:
		explicit StructuredDataMemoryManagementSubService(MemoryManagementService& parent);

		/**
		 * @brief Load structured data into memory
		 * @details Loads structured data into the specified memory area
		 * @param request The TC[6,1] request message
		 */
		void loadObjectMemoryData(Message& request);

		/**
		 * @brief Dump structured memory data
		 * @details Reads structured data from the specified memory area and generates a report
		 * @param request The TC[6,3] request message
		 */
		void dumpObjectMemoryData(Message& request);

		/**
		 * @brief Dumped structured data report
		 * @details Appends to the provided TM[6,4] report the dumped structured data. If the isFinal flag is true, the report is also generated and stored
		 * @attention It is expected, and not checked, that:
		 * 1. the provided report is a TM[6,4] message
		 * 2. it already includes base & N, per the 8.6.2.4 section of the ECSS-E-ST-70-41C standard
		 * @attention Given that the 6.6.4.5 section of the ECSS-E-ST-70-41C standard mentions that all of the instructions should be processed, even if there are 
		 * faulty ones, the TM[6,4] might contain information such as offset = 0 and dataLength = 0. In that case, the next byte will be of the dumbed data of the next instruction.
		 * @param report The TM[6,4] message
		 * @param filePath The path of the file to be dumped
		 * @param offset The offset of the dumped structured data
		 * @param dataLength The length of the dumped structured data
		 * @param isFinal Whether more data is expected or not to be appended to the report. If true, the report is also generated and stored.
		 * @returns true if the appending of new data was successful
		 */
		bool dumpedStructuredDataReport(Message& report, FilePath filePath, Offset offset, FileDataLength dataLength, bool isFinal);

	} structuredDataMemoryManagementSubService;

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
	 * Helper struct to define upper and lower limits of different memories
	 */
	struct MemoryLimits {
		uint32_t lowerLim;
		uint32_t upperLim;
	};

	/**
	 * Map containing all the different types of memory limits
	 */
	inline static const etl::unordered_map<MemoryID, MemoryLimits, MaxMemoryLimitsMapSize> memoryLimitsMap = {
	    {MemoryManagementService::MemoryID::DTCMRAM, {DTCMRAMLowerLim, DTCMRAMUpperLim}},
	    {MemoryManagementService::MemoryID::ITCMRAM, {ITCMRAMLowerLim, ITCMRAMUpperLim}},
	    {MemoryManagementService::MemoryID::RAM_D1, {RAMD1LowerLim, RAMD1UpperLim}},
	    {MemoryManagementService::MemoryID::RAM_D2, {RAMD2LowerLim, RAMD2UpperLim}},
	    {MemoryManagementService::MemoryID::RAM_D3, {RAMD3LowerLim, RAMD3UpperLim}},
	    {MemoryManagementService::MemoryID::FLASH_MEMORY, {FlashLowerLim, FlashUpperLim}}};

	/**
	 * Data structure containing all the valid memory IDs
	 */
	inline static const etl::unordered_set<MemoryID, MaxValidMemoryIdsSize> validMemoryIds = {
	    MemoryManagementService::MemoryID::RAM_D1,
	    MemoryManagementService::MemoryID::RAM_D2,
	    MemoryManagementService::MemoryID::RAM_D3,
	    MemoryManagementService::MemoryID::DTCMRAM,
	    MemoryManagementService::MemoryID::ITCMRAM,
	    MemoryManagementService::MemoryID::FLASH_MEMORY,
	    MemoryManagementService::MemoryID::EXTERNAL};

	/**
	 * Check whether the provided address is valid or not, based on the defined limit values
	 *
	 * @param memId The ID of the memory to check is passed
	 * @param address Takes the address to be checked for validity
	 */
	static bool addressValidator(MemoryManagementService::MemoryID memId, StartAddress address);

	/**
	 * Check if the provided memory ID is valid
	 *
	 * @param memId The memory ID for validation
	 */
	static bool memoryIdValidator(MemoryManagementService::MemoryID memId);

	/**
	 * Validate the data according to checksum calculation
	 */
	static bool dataValidator(const uint8_t* data, MemoryManagementChecksum checksum, MemoryDataLength length);
};

#endif // ECSS_SERVICES_MEMMANGSERVICE_HPP
