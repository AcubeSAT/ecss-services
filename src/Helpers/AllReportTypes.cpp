#include "Helpers/AllReportTypes.hpp"
#include "Services/EventActionService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/HousekeepingService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Services/RealTimeForwardingControlService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/TestService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"

namespace AllReportTypes {
	const ServiceDefinitionsVector ST01Reports = {RequestVerificationService::MessageType::FailedAcceptanceReport,
		                                                                     RequestVerificationService::MessageType::FailedCompletionOfExecution,
		                                                                     RequestVerificationService::MessageType::FailedProgressOfExecution,
		                                                                     RequestVerificationService::MessageType::FailedRoutingReport,
		                                                                     RequestVerificationService::MessageType::FailedStartOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulAcceptanceReport,
		                                                                     RequestVerificationService::MessageType::SuccessfulCompletionOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulProgressOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulStartOfExecution};

	const ServiceDefinitionsVector ST03Reports = {
	    HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::GenerateOneShotHousekeepingReport,
	    HousekeepingService::MessageType::HousekeepingParametersReport,
	    HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
	    HousekeepingService::MessageType::HousekeepingStructuresReport};

	const ServiceDefinitionsVector ST04Reports = {
	    ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport,
	    ParameterStatisticsService::MessageType::ParameterStatisticsReport,
	};

	const ServiceDefinitionsVector ST05Reports = {EventReportService::MessageType::HighSeverityAnomalyReport,
	                                                                         EventReportService::MessageType::DisabledListEventReport,
	                                                                         EventReportService::MessageType::InformativeEventReport,
	                                                                         EventReportService::MessageType::LowSeverityAnomalyReport,
	                                                                         EventReportService::MessageType::MediumSeverityAnomalyReport};

	const ServiceDefinitionsVector ST06Reports = {MemoryManagementService::MessageType::CheckRawMemoryDataReport,
	                                                                         MemoryManagementService::MessageType::DumpRawMemoryDataReport};

	const ServiceDefinitionsVector ST11Reports = {TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport};

	const ServiceDefinitionsVector ST13Reports = {LargePacketTransferService::MessageType::FirstDownlinkPartReport,
	                                                                         LargePacketTransferService::MessageType::InternalDownlinkPartReport,
	                                                                         LargePacketTransferService::MessageType::LastDownlinkPartReport};

	const ServiceDefinitionsVector ST17Reports = {TestService::MessageType::AreYouAliveTestReport,
	                                                                         TestService::MessageType::OnBoardConnectionTestReport};

	const ServiceDefinitionsVector ST19Reports = {EventActionService::MessageType::EventActionStatusReport};

	const ServiceDefinitionsVector ST20Reports = {ParameterService::MessageType::ParameterValuesReport};

	const etl::map<uint8_t, const ServiceDefinitionsVector&, ECSSMaxServiceTypeDefinitions> MessagesOfService = {
	    {RequestVerificationService::ServiceType, ST01Reports},
	    {HousekeepingService::ServiceType, ST03Reports},
	    {ParameterStatisticsService::ServiceType, ST04Reports},
	    {EventReportService::ServiceType, ST05Reports},
	    {MemoryManagementService::ServiceType, ST06Reports},
	    {TimeBasedSchedulingService::ServiceType, ST11Reports},
	    {LargePacketTransferService::ServiceType, ST13Reports},
	    {TestService::ServiceType, ST17Reports},
	    {EventActionService::ServiceType, ST19Reports},
	    {ParameterService::ServiceType, ST20Reports}};

} // namespace AllReportTypes
