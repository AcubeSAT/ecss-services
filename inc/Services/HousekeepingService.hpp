#ifndef ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP
#define ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP

#include "Service.hpp"
#include "Parameter.hpp"
#include "Helpers/TimeHelper.hpp"
#include <etl/vector.h>
#include <etl/map.h>

#include <utility>
#include "Services/ParameterService.hpp"

/**
 * A struct created only for testing in order to access the private members of the class Housekeeping
 *
 * @todo Remove it when the time comes
 */
struct Tester;

/**
 * Implementation of ST[03] housekeeping
 *
 * @details The main purpose of this service is to create housekeeping structures, each one of which contain a group
 * of some parameter IDs. The values of these requested params will be reported periodically. The frequency of the
 * generation of these reports depends on:
 * 1. The time required to call the function that is responsible to generate reports
 * 2  The collection interval, which is the time interval for sampling parameters. The time required to call the
 * corresponding function to generate parameter reports (TM[3,25]) should be <= the minimum collection interval
 * @note The current implementation includes a part of the housekeeping subservice
 * @todo Do we need more than 1 byte for the number of the housekeeping structures?
 */
class HousekeepingService : public Service {
private:
	typedef uint8_t HousekeepingIdType;

	/**
	 * A struct that resembles the housekeeping structures
	 *
	 * @property collectionInterval The time interval for sampling the parameters
	 * @property isPeriodic Indicates if the periodic generation of parameter reports is enabled or disabled. The
	 * default value should be disabled
	 * @property paramId A container to store the requested param IDs
	 * @property timestamp The last time a structure is used for its paramIDs by the TM[3,25]. We use this timestamp to
	 * define If the required time has passed to reuse it
	 * @note The \p collectionInterval should be integer multiple of the time unit of the Real Time Clock
	 * @note The \p collectionInterval should be expressed as units of the minimum sampling interval, refer to
	 * requirement 5.4.3.2c.
	 */
	struct HousekeepingReportStructure {
		uint16_t collectionInterval = 0;
        etl::vector<ParamIdType, ECSS_ST_20_MAX_PARAMETERS> paramId;
        bool isPeriodic = true;

        explicit HousekeepingReportStructure() = default;

        HousekeepingReportStructure(uint16_t collectionInterval, etl::vector<ParamIdType, ECSS_ST_20_MAX_PARAMETERS> paramId,
                                    bool isPeriodic = true) : collectionInterval(collectionInterval), paramId(std::move(paramId)),
                                                       isPeriodic(isPeriodic) {}

    private:
		uint32_t timestamp = 0;

		friend HousekeepingService;
	};

	etl::map<HousekeepingIdType, HousekeepingReportStructure, ECSS_ST_03_MAX_HOUSEKEEPING_STRUCTURES>
	housekeepingStructureList;

	friend struct Tester;
public:
	/**
	 * Define the type of the service
	 */
	HousekeepingService();

	void addHousekeepingStructure(HousekeepingIdType id, const HousekeepingReportStructure & structure);

	/**
	 * TC[3,1] create a housekeeping report structure
	 *
	 * @note Rejected requests due to existed housekeeping structure ID shall generate a failed start of execution
	 * @note The current implementation doesn't allow to override a structure with an already assigned housekeeping ID.
	 * You need first to delete the structure and then create a new one
	 * @note Housekeeping structures can be created either by telecommand (TC[3,1]) or can be predefined on-board
	 * @note The TC[3,1] request can contain only one housekeeping structure. Multiple structures need multiple requests
	 * @note When creating a housekeeping structure the periodic generation should be disabled, as the standard claims
	 * @param message The TC[3,1] request
	 * @todo Should we be able to update an already assigned housekeeping structure?
	 */
	void createHousekeepingStructure(Message& message);

	/**
	 * TC[3,3] delete a housekeeping report structure
	 *
	 * @param message The TC[3,3] request
	 */
	void deleteHousekeepingStructure(Message& message);

	/**
	 * TC[3,5] enable the periodic generation of housekeeping parameter reports
	 *
	 * @param message The TC[3,5] request
	 */
	void enablePeriodicParamReports(Message& message);

	/**
	 * TC[3,6] disable the periodic generation of housekeeping parameter reports
	 *
	 * @param message The TC[3,6] request
	 */
	void disablePeriodicParamReports(Message& message);

	/**
	 * TM[3,25] housekeeping parameter report
	 *
	 * @details This function is the essence of the ST[03] service. It creates multiple telemetry messages that contain
	 * the values of the requested param IDs that are configured on the existing enabled housekeeping structure IDs.
	 * Also, it is supposed to be called periodically to fetch the parameters values in different time moments
	 * @note In order to generate a TM, at least one structure should be enabled. So, the action order is 1) create
	 * structure, 2) enable some of them and 3) call this function to generate reports based on the collection
	 * interval of the enabled structures and the current time
	 * @note The values of the parameters that will be sent are ordered by the way that have been ordered in the
	 * housekeeping structure when it was created.
	 * @param time A preliminary dummy value. It should be fetched from the Real Time Clock (UTC)
	 * @todo Update parameterService in order to have access to parameter values without using friend class
	 * @todo Parameters values aren't a fixed size, so they should be handled accordingly
	 */
	void checkAndSendHousekeepingReports(TimeAndDate time);

	/**
	 * It is responsible to call the suitable function that executes a telecommand contained in a packet. The source of
	 * that packet is the ground station.
	 *
	 * @note This function is called from the main execute() that is defined in the file MessageParser.hpp
	 * @param message Contains the necessary parameters to call the suitable subservice
	 */
	void execute(Message& message);
};

#endif //ECSS_SERVICES_HOUSEKEEPINGSERVICE_HPP
