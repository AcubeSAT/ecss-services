#ifndef ECSS_SERVICES_MEMMANGSERVICE_HPP
#define ECSS_SERVICES_MEMMANGSERVICE_HPP

// Memory limits definitions
#define FIRST_ADRESS_FLASH  0x08000000
#define LAST_ADDRESS_FLASH  0x0801FFFF  // todo: Define the last memory address based on the MCU
#define FIRST_ADDRESS_SRAM  0x20000000
#define SRAM_MEMORY_SIZE    16          // Specify the RAM size in kBytes


#include "Service.hpp"
#include <memory>
#include <iostream>

class MemoryManagementService : public Service {
public:
	// Memory type ID's
	enum MemoryID {
		RAM = 0,
		FLASH = 1,
		EXTERNAL = 2
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
};

#endif //ECSS_SERVICES_MEMMANGSERVICE_HPP
