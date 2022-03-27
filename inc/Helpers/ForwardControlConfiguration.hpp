#ifndef ECSS_SERVICES_FORWARDCONTROLCONFIGURATION_HPP
#define ECSS_SERVICES_FORWARDCONTROLCONFIGURATION_HPP

#include "ECSS_Definitions.hpp"
#include "ErrorHandler.hpp"
#include "etl/vector.h"
#include "etl/map.h"
#include "Helpers/Parameter.hpp"

namespace ForwardControlConfiguration
{
/**
 * The Application Process configuration. Its architecture is based on a 3-level hierarchy, where the first level
 * is the Application process definitions. Each Application Process definition contains a list of Service Type
 * definitions (level 2) and each Service Type definition contains a list of the Report Type definitions (level 3).
 *
 * 					Applications	[][][]
 * 									 /
 * 			Service types		[][][][][][]
 * 									/
 * 							[][][][][]		Report types
 */
class ApplicationProcess {
public:
	/**
	 * Boolean values for each service type of an application process. See next documentation comment.
	 */
	typedef etl::map<uint8_t, bool, ECSSMaxServiceTypeDefinitions> reportsAreNotEmpty;

	/**
	 * Indicates the meaning of a service type being empty of report types. It specifies whether the 'empty' means that
	 * it has not been filled yet (so we proceed to add new report types) or it is empty because we accept every
	 * report type for the service.
	 * TODO: rename to 'allServicesAllowed'
	 */
	etl::map<uint8_t, reportsAreNotEmpty, ECSSMaxControlledApplications> serviceNotEmpty;

//	etl::map<uint8_t, bool, ECSSMaxControlledApplications> serviceOfAppNotEmpty;

	/**
	 * Vector containing the Report Type definitions. Each definition has its unique name of type uint8. For
	 * example, a Report Type definition could be 'ReportHousekeepingStructures'.
	 */
	typedef etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> reportTypeDefinitions;

	/**
	 * Map containing the Service Type definitions. Each Service Type definition is accessed via its key-name, of type
	 * uint8, for example '20' for the  Parameter Management service. Each Service Type definition, contains the
	 * list of its own Report Type definitions.
	 */
	typedef etl::map<uint8_t, reportTypeDefinitions, ECSSMaxServiceTypeDefinitions> serviceTypeDefinitions;

	/**
	 * Map containing the Application Process definitions. Each application has its own ID. The ID is used as a
	 * key to provide access to the list of Service Type definitions, included by the application.
	 */
	etl::map<uint8_t, serviceTypeDefinitions, ECSSMaxControlledApplications> definitions;
	ApplicationProcess() = default;
};

/**
 * The Housekeeping Parameter Report configuration. Its architecture is based on a 2-level hierarchy, where the
 * first level is the Application process definitions. Each application process definition contains a list of
 * Housekeeping structure IDs (level 2).
 *
 * 					Applications	[][][]
 * 									 /
 * 								[][][][][][]	Housekeeping structure IDs
 */
class HousekeepingParameterReport {
public:
	/**
	 * Vector containing the Housekeeping structure IDs.
	 */
	typedef etl::vector<uint8_t, ECSSMaxHousekeepingStructureIDs> housekeepingStructureIds;

	/**
	 * Map containing the Housekeeping definitions (application processes). Each application has its own ID. The ID is
	 * used as a key to provide access to the list of the Housekeeping structure IDs.
	 */
	typedef etl::map<uint8_t, housekeepingStructureIds, ECSSMaxControlledApplications> definitions;
};

/**
 * The Housekeeping Parameter Report configuration. Its architecture is based on a 2-level hierarchy, where the
 * first level is the Application process definitions. Each application process definition contains a list of
 * Event Definition IDs (level 2).
 *
 * 					Applications	[][][]
 * 									 /
 * 								[][][][][][]	Event Definition IDs
 */
class EventReportBlocking {
public:
	/**
	 * Vector containing the Event Definition IDs.
	 */
	typedef etl::vector<uint8_t, ECSSMaxEventDefinitionIDs> eventDefinitionIds;

	/**
	 * Map containing the Event Report Blocking definitions (applications). Each application has its own ID. The ID is
	 * used as a key to provide access to the list of the Event Definitions.
	 */
	typedef etl::map<uint8_t, eventDefinitionIds, ECSSMaxControlledApplications> definitions;
};

} // namespace ForwardControlConfiguration

#endif
