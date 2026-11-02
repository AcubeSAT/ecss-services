#include <algorithm>
#include <iterator>
#include "ECSS_Configuration.hpp"
#include "Helpers/AllReportTypes.hpp"
#include "Services/EventActionService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/FileManagementService.hpp"
#include "Services/HousekeepingService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/OnBoardMonitoringService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Services/RealTimeForwardingControlService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/StorageAndRetrievalService.hpp"
#include "Services/TestService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"

namespace AllReportTypes {
	namespace {
		[[maybe_unused]] constexpr MessageTypeNum ST01Reports[] = {
		    RequestVerificationService::MessageType::FailedAcceptanceReport,
		    RequestVerificationService::MessageType::FailedCompletionOfExecution,
		    RequestVerificationService::MessageType::FailedProgressOfExecution,
		    RequestVerificationService::MessageType::FailedRoutingReport,
		    RequestVerificationService::MessageType::FailedStartOfExecution,
		    RequestVerificationService::MessageType::SuccessfulAcceptanceReport,
		    RequestVerificationService::MessageType::SuccessfulCompletionOfExecution,
		    RequestVerificationService::MessageType::SuccessfulProgressOfExecution,
		    RequestVerificationService::MessageType::SuccessfulStartOfExecution};

		[[maybe_unused]] constexpr MessageTypeNum ST03Reports[] = {
		    HousekeepingService::MessageType::HousekeepingParametersReport,
		    HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
		    HousekeepingService::MessageType::HousekeepingStructuresReport};

		[[maybe_unused]] constexpr MessageTypeNum ST04Reports[] = {
		    ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport,
		    ParameterStatisticsService::MessageType::ParameterStatisticsReport};

		[[maybe_unused]] constexpr MessageTypeNum ST05Reports[] = {
		    EventReportService::MessageType::HighSeverityAnomalyReport,
		    EventReportService::MessageType::DisabledListEventReport,
		    EventReportService::MessageType::InformativeEventReport,
		    EventReportService::MessageType::LowSeverityAnomalyReport,
		    EventReportService::MessageType::MediumSeverityAnomalyReport};

		[[maybe_unused]] constexpr MessageTypeNum ST06Reports[] = {
		    MemoryManagementService::MessageType::CheckRawMemoryDataReport,
		    MemoryManagementService::MessageType::DumpRawMemoryDataReport};

		[[maybe_unused]] constexpr MessageTypeNum ST11Reports[] = {
		    TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport};

		[[maybe_unused]] constexpr MessageTypeNum ST12Reports[] = {
		    OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionReport,
		    OnBoardMonitoringService::MessageType::OutOfLimitsReport,
		    OnBoardMonitoringService::MessageType::CheckTransitionReport,
		    OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionStatusReport};

		[[maybe_unused]] constexpr MessageTypeNum ST13Reports[] = {
		    LargePacketTransferService::MessageType::FirstDownlinkPartReport,
		    LargePacketTransferService::MessageType::InternalDownlinkPartReport,
		    LargePacketTransferService::MessageType::LastDownlinkPartReport};

		[[maybe_unused]] constexpr MessageTypeNum ST14Reports[] = {
		    RealTimeForwardingControlService::MessageType::AppProcessConfigurationContentReport,
		    RealTimeForwardingControlService::MessageType::EventReportConfigurationContentReport};

		[[maybe_unused]] constexpr MessageTypeNum ST15Reports[] = {
		    StorageAndRetrievalService::MessageType::ApplicationProcessReport,
		    StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport,
		    StorageAndRetrievalService::MessageType::PacketStoresStatusReport,
		    StorageAndRetrievalService::MessageType::PacketStoreConfigurationReport};

		[[maybe_unused]] constexpr MessageTypeNum ST17Reports[] = {
		    TestService::MessageType::AreYouAliveTestReport,
		    TestService::MessageType::OnBoardConnectionTestReport};

		[[maybe_unused]] constexpr MessageTypeNum ST19Reports[] = {
		    EventActionService::MessageType::EventActionStatusReport};

		[[maybe_unused]] constexpr MessageTypeNum ST20Reports[] = {
		    ParameterService::MessageType::ParameterValuesReport};

		[[maybe_unused]] constexpr MessageTypeNum ST23Reports[] = {
		    FileManagementService::MessageType::CreateAttributesReport,
		    FileManagementService::MessageType::FoundFileReport,
		    FileManagementService::MessageType::SummaryDirectoryReport,
		    FileManagementService::MessageType::FileCopyStatusReport};

		/**
		 * The report-generating services enabled in this build, with their report types.
		 */
		constexpr ReportTypesPerService::value_type ReportingServices[] = {
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

		constexpr size_t mostReportTypes() {
			size_t most = 0;
			for (const auto& service: ReportingServices) {
				most = std::max(most, service.second.size());
			}
			return most;
		}

		static_assert(std::size(ReportingServices) <= ECSSMaxServiceTypeDefinitions, "Adding all report types of an application adds every service in MessagesOfService without a limit check");
		static_assert(mostReportTypes() <= ECSSMaxReportTypeDefinitions, "A service has more report types than a report type definition can hold");
	} // namespace

	const ReportTypesPerService MessagesOfService(std::begin(ReportingServices), std::end(ReportingServices));

} // namespace AllReportTypes
