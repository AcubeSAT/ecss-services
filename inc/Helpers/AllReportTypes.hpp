#pragma once
#include "etl/vector.h"
#include "etl/map.h"
#include "ECSS_Definitions.hpp"
#include "TypeDefinitions.hpp"

/**
 * Namespace containing all the report types for every service type.
 * For use in ST[14] and ST[15]
 */
namespace AllReportTypes {
	/**
	 * Vector to contain all report types defined for a service
	 */
	using ServiceDefinitionsVector = etl::vector<MessageTypeNum, ECSSMaxReportTypeDefinitions>;

	// Function declarations for getting service reports
	const ServiceDefinitionsVector& getST01Reports();
	const ServiceDefinitionsVector& getST03Reports();
	const ServiceDefinitionsVector& getST04Reports();
	const ServiceDefinitionsVector& getST05Reports();
	const ServiceDefinitionsVector& getST06Reports();
	const ServiceDefinitionsVector& getST11Reports();
	const ServiceDefinitionsVector& getST12Reports();
	const ServiceDefinitionsVector& getST13Reports();
	const ServiceDefinitionsVector& getST14Reports();
	const ServiceDefinitionsVector& getST15Reports();
	const ServiceDefinitionsVector& getST17Reports();
	const ServiceDefinitionsVector& getST19Reports();
	const ServiceDefinitionsVector& getST20Reports();
	const ServiceDefinitionsVector& getST23Reports();

	/**
	 * Returns a reference to the map containing all report types, per service. 
	 * The key is the ServiceType and the value is a vector containing the report types.
	 */
	const etl::map<uint8_t, ServiceDefinitionsVector, ECSSMaxServiceTypeDefinitions>& getMessagesOfService();

} // namespace AllReportTypes