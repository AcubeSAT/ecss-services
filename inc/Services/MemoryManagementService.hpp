#ifndef ECSS_SERVICES_MEMMANGSERVICE_HPP
#define ECSS_SERVICES_MEMMANGSERVICE_HPP

// Memory limits definitions
#define DTCMRAM_LOWER_LIM 0x20000000UL
#define DTCMRAM_UPPER_LIM 0x20020000UL
#define ITCMRAM_LOWER_LIM 0x00000000UL
#define ITCMRAM_UPPER_LIM 0x00010000UL
#define RAM_D1_LOWER_LIM 0x24000000UL
#define RAM_D1_UPPER_LIM 0x24080000UL
#define RAM_D2_LOWER_LIM 0x30000000UL
#define RAM_D2_UPPER_LIM 0x30048000UL
#define RAM_D3_LOWER_LIM 0x38000000UL
#define RAM_D3_UPPER_LIM 0x38010000UL
#define FLASH_LOWER_LIM 0x08000000UL
#define FLASH_UPPER_LIM 0x08200000UL


#include <memory>
#include <iostream>
#include "Service.hpp"
#include "Services/RequestVerificationService.hpp"


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
		EXTERNAL
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
		MemoryManagementService &mainService; // Used to access main class's members

	public:
		explicit RawDataMemoryManagement(MemoryManagementService &parent);

		/**
		 * TC[6,2] load raw values to memory
		 *
		 * @details This function loads new values to memory data areas
		 * 			specified in the request
		 * @param request: Provide the received message as a parameter
		 * @todo Only allow aligned memory address to be start addresses
		 */
		void loadRawData(Message &request);

		/**
		 * TC[6,5] read raw memory values
		 *
		 * @details This function reads the raw data from the RAM memory and
		 * 			triggers a TM[6,6] report
		 * @param request: Provide the received message as a parameter
		 * @todo In later embedded version, implement error checking for address validity for
		 * 		 different memory types
		 * @todo Only allow aligned memory address to be start addresses
		 */
		void dumpRawData(Message &request);

		/**
		 * TC[6,9] check raw memory data
		 *
		 * @details This function reads the raw data from the specified memory and
		 * 			triggers a TM[6,10] report
		 * @param request: Provide the received message as a parameter
		 * @todo In later embedded version, implement error checking for address validity for
		 * 		 different memory types
		 * @todo Only allow aligned memory address to be start addresses
		 */
		void checkRawData(Message &request);
	} rawDataMemorySubservice;

private:
	RequestVerificationService requestVerificationService;

	/**
		 * Check whether the provided address is valid or not, based on the defined limit values
		 *
		 * @param memId: The ID of the memory to check is passed
		 * @param address: Takes the address to be checked for validity
		 */
	bool addressValidator(MemoryManagementService::MemoryID memId, uint64_t address);

	/**
	 * Check if the provided memory ID is valid
	 *
	 * @param memId: The memory ID for validation
	 */
	bool memoryIdValidator(MemoryManagementService::MemoryID memId);
};

#endif //ECSS_SERVICES_MEMMANGSERVICE_HPP
