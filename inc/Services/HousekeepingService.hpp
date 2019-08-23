#ifndef ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP

#include "Service.hpp"
#include "Parameter.hpp"
#include "Helpers/TimeHelper.hpp"
#include <etl/vector.h>
#include <etl/map.h>
#include "Services/ParameterService.hpp"

#define MAX_HOUSEKEEPING_STRUCTURES 3u

struct Tester; // only for testing in order to access the private members of the class Housekeeping

/**
 * Implementation of ST[03] housekeeping
 *
 * @details The main purpose of this report is to create housekeeping structures, that each one of those will contain a
 * group of some parameter IDs and to report periodically the values of these requested params. How frequent these
 * report will be generated, depends on the 1)time required to call the function that is responsible to generate
 * reports and the 2) collection interval. The collection interval is the time interval for sampling parameters. The
 * time required to call the corresponding function to generate parameter reports (TM[3,25]) should be <= the minimum
 * collection interval
 * @note The current implementation includes only the housekeeping subservice
 */
class HousekeepingService : public Service {
private:
	typedef uint8_t HousekeepingId; // @todo do we need more than 1 byte

	/**
	 * A struct that resembles the housekeeping structures
	 *
	 * @var collectionInterval The time interval for sampling the parameters in seconds
	 * @var periodicStatus Indicates if the periodic generation of parameter reports is enabled or disabled. The
	 * default value should be disabled
	 * @var paramId A container to store the requested param IDs
	 * @var timestamp the last time a structure is used for its paramIDs by the TM[3,25]. We use this timestamp to
	 * define If the required time have passed to reuse it
	 * @note We assume that the time unit for the Real Time Clock is seconds. So the \p collection intervals should be
	 * integer multiple of seconds
	 * @note The \p collection interval should be >= the sample interval. The sample interval is the time required to
	 * update the values of the parameters
	 */
	struct HousekeepingReportStructure {
		uint16_t collectInterval = 0;
		bool periodicStatus = false; //
		etl::vector<ParamId, MAX_PARAMS> paramId;
		uint32_t timestamp = 0;
	};


	 // Map is used as a container for the housekeeping structure to be more efficient the searching 1)when checking
	 // if a structure with an existed ID is requested to be created and 2)for the deletion of a structure based on
	 // its id
	etl::map<HousekeepingId, HousekeepingReportStructure, MAX_HOUSEKEEPING_STRUCTURES> housekeepingStructureList;

	friend struct Tester;
public:
	/**
	 * Initialize the serviceType
	 */
	HousekeepingService();

	/**
	 * TC[3,1] create a housekeeping report structure
	 *
	 * @note Rejected requests due to existed housekeeping structure ID shall generate a failed start of execution
	 * @note The current implementation doesn't allow to override a structure with an already assigned housekeeping ID.
	 * You need first to delete the structure and then create a new one
	 * @note Housekeeping structures can be created either by telecommand (TC[3,1]) or can be predefined on-board
	 * @note When creating a housekeeping structure the periodic generation should be disabled, as the standard claims
	 * @todo Should we be able to update an already assigned housekeeping structure?
	 */
	void createHousekeepingStructure(Message& message);

	/**
	 * TC[3,3] delete a housekeeping report structure
	 */
	void deleteHousekeepingStructure(Message& message);

	/**
	 * TC[3,5] enable the periodic generation of housekeeping parameter reports
	 */
	void enablePeriodParamReports(Message& message);

	/**
	 * TC[3,6] disable the periodic generation of housekeeping parameter reports
	 */
	void disablePeriodParamReports(Message& message);

	/**
	 * TM[3,25] housekeeping parameter report
	 *
	 * @details This function is the essence of the ST[03] service. It creates a telemetry message that contains the
	 * values of the requested param IDs that are configured on the existing housekeeper structure IDs
	 * @note In order to generate a TM, at least one structure should exist and at least one should be enabled
	 * @note the values of the parameters that will be sent are ordered by its paramID
	 * @param currTime A preliminary dummy value. It should be fetched from the Real Time Clock (UTC)
	 * @todo Update parameterService in order to have access to parameter values without using friend class
	 */
	void paramReport(TimeAndDate time);

	/**
	 * It is responsible to call the suitable function that execute the proper subservice. The
	 * way that the subservices are selected is for the time being based on the messageType(class
	 * member of class Message) of the param message
	 *
	 * @todo Error handling for the switch() in the implementation of this execute function
	 */
	void execute(Message& message);
};

#endif //ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP
