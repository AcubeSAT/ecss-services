#include "Helpers/AllMessageTypes.hpp"
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

namespace AllMessageTypes {
	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST01Messages = {RequestVerificationService::MessageType::FailedAcceptanceReport,
		                                                                     RequestVerificationService::MessageType::FailedCompletionOfExecution,
		                                                                     RequestVerificationService::MessageType::FailedProgressOfExecution,
		                                                                     RequestVerificationService::MessageType::FailedRoutingReport,
		                                                                     RequestVerificationService::MessageType::FailedStartOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulAcceptanceReport,
		                                                                     RequestVerificationService::MessageType::SuccessfulCompletionOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulProgressOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulStartOfExecution};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST03Messages = {
	    HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::GenerateOneShotHousekeepingReport,
	    HousekeepingService::MessageType::HousekeepingParametersReport,
	    HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
	    HousekeepingService::MessageType::HousekeepingStructuresReport};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST04Messages = {
	    ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport,
	    ParameterStatisticsService::MessageType::ParameterStatisticsReport,
	};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST05Messages = {EventReportService::MessageType::HighSeverityAnomalyReport,
	                                                                         EventReportService::MessageType::DisabledListEventReport,
	                                                                         EventReportService::MessageType::InformativeEventReport,
	                                                                         EventReportService::MessageType::LowSeverityAnomalyReport,
	                                                                         EventReportService::MessageType::MediumSeverityAnomalyReport};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST06Messages = {MemoryManagementService::MessageType::CheckRawMemoryDataReport,
	                                                                         MemoryManagementService::MessageType::DumpRawMemoryDataReport};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST11Messages = {TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST13Messages = {LargePacketTransferService::MessageType::FirstDownlinkPartReport,
	                                                                         LargePacketTransferService::MessageType::InternalDownlinkPartReport,
	                                                                         LargePacketTransferService::MessageType::LastDownlinkPartReport};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST17Messages = {TestService::MessageType::AreYouAliveTestReport,
	                                                                         TestService::MessageType::OnBoardConnectionTestReport};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST19Messages = {EventActionService::MessageType::EventActionStatusReport};

	const etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> ST20Messages = {ParameterService::MessageType::ParameterValuesReport};

	const etl::map<uint8_t, etl::vector<uint8_t, ECSSMaxReportTypeDefinitions>, ECSSMaxServiceTypeDefinitions> MessagesOfService = {
	    {RequestVerificationService::ServiceType, ST01Messages},
	    {HousekeepingService::ServiceType, ST03Messages},
	    {ParameterStatisticsService::ServiceType, ST04Messages},
	    {EventReportService::ServiceType, ST05Messages},
	    {MemoryManagementService::ServiceType, ST06Messages},
	    {TimeBasedSchedulingService::ServiceType, ST11Messages},
	    {LargePacketTransferService::ServiceType, ST13Messages},
	    {TestService::ServiceType, ST17Messages},
	    {EventActionService::ServiceType, ST19Messages},
	    {ParameterService::ServiceType, ST20Messages}};

} // namespace AllMessageTypes
