#pragma once
#include "etl/flat_map.h"
#include "etl/span.h"
#include "ECSS_Definitions.hpp"
#include "Helpers/TypeDefinitions.hpp"

/**
 * Namespace containing all the report types for every service type.
 * For use in ST[14] and ST[15]
 *
 * @note
 * This deviates from ECSS-E-ST-70-41C on purpose. In ECSS 6.14.3.2.2 and 6.15.4.2.2, the subservice has no knowledge of
 * the existing report types, and an empty list of definitions means that everything is allowed. Here, "add all report
 * types" is expanded to an explicit list from this table, which lists the report-generating services enabled in this
 * build. Those of a subsystem whose reports are controlled from here must be enabled here too.
 */
namespace AllReportTypes {
	/**
	 * The report types defined for a service.
	 */
	using ReportTypes = etl::span<const MessageTypeNum>;

	using ReportTypesPerService = etl::flat_map<ServiceTypeNum, ReportTypes, ECSSMaxServiceTypeDefinitions>;

	/**
	 * Flat map containing all the report types, per service. The key is the ServiceType and the value
	 * is a span over the report types.
	 */
	extern const ReportTypesPerService MessagesOfService;

} // namespace AllReportTypes
