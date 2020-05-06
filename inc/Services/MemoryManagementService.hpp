#ifndef ECSS_SERVICES_MEMMANGSERVICE_HPP
#define ECSS_SERVICES_MEMMANGSERVICE_HPP

#include <memory>
#include "Service.hpp"
#include "Helpers/CRCHelper.hpp"
#include "ErrorHandler.hpp"
#include "Platform/STM32F7/MemoryAddressLimits.hpp"

/**
 * @ingroup Services
 */
class MemoryManagementService : public Service {
public:
	// Memory type ID's
	enum MemoryID {
		DTCMRAM = 0,
		RAM_D1,
		RAM_D2,
		RAM_D3,
		ITCMRAM,
		FLASH,
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
	class RawDataMemoryManagement {
	private:
		MemoryManagementService& mainService; // Used to access main class's members

	public:
		explicit RawDataMemoryManagement(MemoryManagementService& parent);

		/**
		 * TC[6,2] load raw values to memory
		 *
		 * @details This function loads new values to memory data areas
		 * 			specified in the request
		 * @param request Provide the received message as a parameter
		 * @todo Only allow aligned memory address to be start addresses
		 */
		void loadRawData(Message& request);

		/**
		 * TC[6,5] read raw memory values
		 *
		 * @details This function reads the raw data from the RAM memory and
		 * 			triggers a TM[6,6] report
		 * @param request Provide the received message as a parameter
		 * @todo In later embedded version, implement error checking for address validity for
		 * 		 different memory types
		 * @todo Only allow aligned memory address to be start addresses
		 */
		void dumpRawData(Message& request);

		/**
		 * TC[6,9] check raw memory data
		 *
		 * @details This function reads the raw data from the specified memory and
		 * 			triggers a TM[6,10] report
		 * @param request Provide the received message as a parameter
		 * @todo In later embedded version, implement error checking for address validity for
		 * 		 different memory types
		 * @todo Only allow aligned memory address to be start addresses
		 */
		void checkRawData(Message &request);
	} rawDataMemorySubservice;

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
	 * Check whether the provided address is valid or not, based on the defined limit values
	 *
	 * @param memId The ID of the memory to check is passed
	 * @param address Takes the address to be checked for validity
	 */
	bool addressValidator(MemoryManagementService::MemoryID memId, uint64_t address);

	/**
	 * Check if the provided memory ID is valid
	 *
	 * @param memId The memory ID for validation
	 */
	bool memoryIdValidator(MemoryManagementService::MemoryID memId);

	/**
	 * Validate the data according to checksum calculation
	 *
	 */
	bool dataValidator(const uint8_t* data, uint16_t checksum, uint16_t length);
};

#endif // ECSS_SERVICES_MEMMANGSERVICE_HPP
