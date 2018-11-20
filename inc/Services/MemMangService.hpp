#ifndef ECSS_SERVICES_MEMMANGSERVICE_HPP
#define ECSS_SERVICES_MEMMANGSERVICE_HPP

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
		/**
		 * TM[6,6] dumped raw memory data report
		 *
		 * @details This report is triggered through TC[6,5]
		 */
		void dumpedRawDataReport();
		MemoryManagementService *mainService; // Used to access main class's members

	public:
		explicit RawDataMemoryManagement(MemoryManagementService *parent);
		/**
		 * TC[6,2] load raw values to memory
		 *
		 * @details This function loads new values to memory data areas
		 * 			specified in the request
		 * @param request: Provide the received message as a parameter
		 */
		void loadRawData(Message &request);

		/**
		 * TC[6,5] read raw memory values
		 *
		 * @details This function reads the raw data from the RAM memory and
		 * 			triggers a TM[6,6] report
		 * @param request: Provide the received message as a parameter
		 */
		void dumpRawData(Message &request);
	} rawDataMemorySubservice;
};

#endif //ECSS_SERVICES_MEMMANGSERVICE_HPP
