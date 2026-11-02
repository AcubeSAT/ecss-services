#include "Helpers/AllReportTypes.hpp"
#ifdef SERVICE_EVENTACTION
#include "Services/EventActionService.hpp"
#endif
#ifdef SERVICE_EVENTREPORT
#include "Services/EventReportService.hpp"
#endif
#ifdef SERVICE_FILE_MANAGEMENT
#include "Services/FileManagementService.hpp"
#endif
#ifdef SERVICE_HOUSEKEEPING
#include "Services/HousekeepingService.hpp"
#endif
#ifdef SERVICE_LARGEPACKET
#include "Services/LargePacketTransferService.hpp"
#endif
#ifdef SERVICE_MEMORY
#include "Services/MemoryManagementService.hpp"
#endif
#ifdef SERVICE_ONBOARDMONITORING
#include "Services/OnBoardMonitoringService.hpp"
#endif
#ifdef SERVICE_PARAMETER
#include "Services/ParameterService.hpp"
#endif
#ifdef SERVICE_PARAMETERSTATISTICS
#include "Services/ParameterStatisticsService.hpp"
#endif
#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
#include "Services/RealTimeForwardingControlService.hpp"
#endif
#ifdef SERVICE_REQUESTVERIFICATION
#include "Services/RequestVerificationService.hpp"
#endif
#ifdef SERVICE_STORAGEANDRETRIEVAL
#include "Services/StorageAndRetrievalService.hpp"
#endif
#ifdef SERVICE_TEST
#include "Services/TestService.hpp"
#endif
#ifdef SERVICE_TIMESCHEDULING
#include "Services/TimeBasedSchedulingService.hpp"
#endif

namespace AllReportTypes {
#ifdef SERVICE_REQUESTVERIFICATION
	const ServiceDefinitionsVector ST01Reports = {RequestVerificationService::MessageType::FailedAcceptanceReport,
		                                                                     RequestVerificationService::MessageType::FailedCompletionOfExecution,
		                                                                     RequestVerificationService::MessageType::FailedProgressOfExecution,
		                                                                     RequestVerificationService::MessageType::FailedRoutingReport,
		                                                                     RequestVerificationService::MessageType::FailedStartOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulAcceptanceReport,
		                                                                     RequestVerificationService::MessageType::SuccessfulCompletionOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulProgressOfExecution,
		                                                                     RequestVerificationService::MessageType::SuccessfulStartOfExecution};
#endif

#ifdef SERVICE_HOUSEKEEPING
	const ServiceDefinitionsVector ST03Reports = {
	    HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::GenerateOneShotHousekeepingReport,
	    HousekeepingService::MessageType::HousekeepingParametersReport,
	    HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
	    HousekeepingService::MessageType::HousekeepingStructuresReport};
#endif

#ifdef SERVICE_PARAMETERSTATISTICS
	const ServiceDefinitionsVector ST04Reports = {
	    ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport,
	    ParameterStatisticsService::MessageType::ParameterStatisticsReport,
	};
#endif

#ifdef SERVICE_EVENTREPORT
	const ServiceDefinitionsVector ST05Reports = {EventReportService::MessageType::HighSeverityAnomalyReport,
	                                                                         EventReportService::MessageType::DisabledListEventReport,
	                                                                         EventReportService::MessageType::InformativeEventReport,
	                                                                         EventReportService::MessageType::LowSeverityAnomalyReport,
	                                                                         EventReportService::MessageType::MediumSeverityAnomalyReport};
#endif

#ifdef SERVICE_MEMORY
	const ServiceDefinitionsVector ST06Reports = {MemoryManagementService::MessageType::CheckRawMemoryDataReport,
	                                                                         MemoryManagementService::MessageType::DumpRawMemoryDataReport};
#endif

#ifdef SERVICE_TIMESCHEDULING
	const ServiceDefinitionsVector ST11Reports = {TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport};
#endif

#ifdef SERVICE_ONBOARDMONITORING
	const ServiceDefinitionsVector ST12Reports = {OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionReport,
	                                              OnBoardMonitoringService::MessageType::OutOfLimitsReport,
	                                              OnBoardMonitoringService::MessageType::CheckTransitionReport,
	                                              OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionStatusReport};
#endif

#ifdef SERVICE_LARGEPACKET
	const ServiceDefinitionsVector ST13Reports = {LargePacketTransferService::MessageType::FirstDownlinkPartReport,
	                                                                         LargePacketTransferService::MessageType::InternalDownlinkPartReport,
	                                                                         LargePacketTransferService::MessageType::LastDownlinkPartReport};
#endif

#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
	const ServiceDefinitionsVector ST14Reports = {RealTimeForwardingControlService::MessageType::AppProcessConfigurationContentReport,
	                                              RealTimeForwardingControlService::MessageType::EventReportConfigurationContentReport};
#endif

#ifdef SERVICE_STORAGEANDRETRIEVAL
	const ServiceDefinitionsVector ST15Reports = {StorageAndRetrievalService::MessageType::ApplicationProcessReport,
	                                              StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport,
	                                              StorageAndRetrievalService::MessageType::PacketStoresStatusReport,
	                                              StorageAndRetrievalService::MessageType::PacketStoreConfigurationReport};
#endif

#ifdef SERVICE_TEST
	const ServiceDefinitionsVector ST17Reports = {TestService::MessageType::AreYouAliveTestReport,
	                                                                         TestService::MessageType::OnBoardConnectionTestReport};
#endif

#ifdef SERVICE_EVENTACTION
	const ServiceDefinitionsVector ST19Reports = {EventActionService::MessageType::EventActionStatusReport};
#endif

#ifdef SERVICE_PARAMETER
	const ServiceDefinitionsVector ST20Reports = {ParameterService::MessageType::ParameterValuesReport};
#endif

#ifdef SERVICE_FILE_MANAGEMENT
	const ServiceDefinitionsVector ST23Reports = {FileManagementService::MessageType::CreateAttributesReport,
	                                              FileManagementService::MessageType::FoundFileReport,
	                                              FileManagementService::MessageType::SummaryDirectoryReport,
	                                              FileManagementService::MessageType::FileCopyStatusReport};
#endif

	static_assert(ECSSMaxServiceTypeDefinitions >= ReportingServiceCount, "Adding all report types of an application adds every service in MessagesOfService without a limit check");

	const etl::flat_map<ServiceTypeNum, const ServiceDefinitionsVector&, ReportingServiceCount> MessagesOfService = {
#ifdef SERVICE_REQUESTVERIFICATION
	    {RequestVerificationService::ServiceType, ST01Reports},
#endif
#ifdef SERVICE_HOUSEKEEPING
	    {HousekeepingService::ServiceType, ST03Reports},
#endif
#ifdef SERVICE_PARAMETERSTATISTICS
	    {ParameterStatisticsService::ServiceType, ST04Reports},
#endif
#ifdef SERVICE_EVENTREPORT
	    {EventReportService::ServiceType, ST05Reports},
#endif
#ifdef SERVICE_MEMORY
	    {MemoryManagementService::ServiceType, ST06Reports},
#endif
#ifdef SERVICE_TIMESCHEDULING
	    {TimeBasedSchedulingService::ServiceType, ST11Reports},
#endif
#ifdef SERVICE_ONBOARDMONITORING
	    {OnBoardMonitoringService::ServiceType, ST12Reports},
#endif
#ifdef SERVICE_LARGEPACKET
	    {LargePacketTransferService::ServiceType, ST13Reports},
#endif
#ifdef SERVICE_REALTIMEFORWARDINGCONTROL
	    {RealTimeForwardingControlService::ServiceType, ST14Reports},
#endif
#ifdef SERVICE_STORAGEANDRETRIEVAL
	    {StorageAndRetrievalService::ServiceType, ST15Reports},
#endif
#ifdef SERVICE_TEST
	    {TestService::ServiceType, ST17Reports},
#endif
#ifdef SERVICE_EVENTACTION
	    {EventActionService::ServiceType, ST19Reports},
#endif
#ifdef SERVICE_PARAMETER
	    {ParameterService::ServiceType, ST20Reports},
#endif
#ifdef SERVICE_FILE_MANAGEMENT
	    {FileManagementService::ServiceType, ST23Reports},
#endif
	};

} // namespace AllReportTypes
