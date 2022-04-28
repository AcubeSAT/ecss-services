#include "Helpers/AllMessageTypes.hpp"

namespace AllMessageTypes {
	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st01Messages = {RequestVerificationService::MessageType::FailedAcceptanceReport,
	                                                                   RequestVerificationService::MessageType::FailedCompletionOfExecution,
	                                                                   RequestVerificationService::MessageType::FailedProgressOfExecution,
	                                                                   RequestVerificationService::MessageType::FailedRoutingReport,
	                                                                   RequestVerificationService::MessageType::FailedStartOfExecution,
	                                                                   RequestVerificationService::MessageType::SuccessfulAcceptanceReport,
	                                                                   RequestVerificationService::MessageType::SuccessfulCompletionOfExecution,
	                                                                   RequestVerificationService::MessageType::SuccessfulProgressOfExecution,
	                                                                   RequestVerificationService::MessageType::SuccessfulStartOfExecution};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st03Messages = {
	    HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::GenerateOneShotHousekeepingReport,
	    HousekeepingService::MessageType::HousekeepingParametersReport,
	    HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
	    HousekeepingService::MessageType::HousekeepingStructuresReport};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st04Messages = {
	    ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport,
	    ParameterStatisticsService::MessageType::ParameterStatisticsReport,
	};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st05Messages = {EventReportService::MessageType::HighSeverityAnomalyReport,
	                                                                   EventReportService::MessageType::DisabledListEventReport,
	                                                                   EventReportService::MessageType::InformativeEventReport,
	                                                                   EventReportService::MessageType::LowSeverityAnomalyReport,
	                                                                   EventReportService::MessageType::MediumSeverityAnomalyReport};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st06Messages = {MemoryManagementService::MessageType::CheckRawMemoryDataReport,
	                                                                   MemoryManagementService::MessageType::DumpRawMemoryDataReport};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st11Messages = {TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st13Messages = {LargePacketTransferService::MessageType::FirstDownlinkPartReport,
	                                                                   LargePacketTransferService::MessageType::InternalDownlinkPartReport,
	                                                                   LargePacketTransferService::MessageType::LastDownlinkPartReport};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st17Messages = {TestService::MessageType::AreYouAliveTestReport,
	                                                                   TestService::MessageType::OnBoardConnectionTestReport};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st19Messages = {EventActionService::MessageType::EventActionStatusReport};

	etl::vector<uint8_t, ECSSMaxReportTypeDefinitions> st20Messages = {ParameterService::MessageType::ParameterValuesReport};

	etl::map<uint8_t, etl::vector<uint8_t, ECSSMaxReportTypeDefinitions>, ECSSMaxServiceTypeDefinitions> messagesOfService = {
	    {RequestVerificationService::ServiceType, st01Messages},
	    {HousekeepingService::ServiceType, st03Messages},
	    {ParameterStatisticsService::ServiceType, st04Messages},
	    {EventReportService::ServiceType, st05Messages},
	    {MemoryManagementService::ServiceType, st06Messages},
	    {TimeBasedSchedulingService::ServiceType, st11Messages},
	    {LargePacketTransferService::ServiceType, st13Messages},
	    {TestService::ServiceType, st17Messages},
	    {EventActionService::ServiceType, st19Messages},
	    {ParameterService::ServiceType, st20Messages}};

} // namespace AllMessageTypes
