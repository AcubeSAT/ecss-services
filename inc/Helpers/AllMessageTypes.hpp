#ifndef ECSS_SERVICES_ALLMESSAGETYPES_HPP
#define ECSS_SERVICES_ALLMESSAGETYPES_HPP

#include "Services/RequestVerificationService.hpp"
#include "Services/HousekeepingService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/RealTimeForwardingControlService.hpp"
#include "Services/TestService.hpp"
#include "Services/EventActionService.hpp"
#include "Services/ParameterService.hpp"

/**
 * Class holding all the message types for every service type.
 */
class AllMessageTypes {
public:
	etl::vector<uint8_t, 15> st01Messages = {RequestVerificationService::MessageType::FailedAcceptanceReport,
	                                         RequestVerificationService::MessageType::FailedCompletionOfExecution,
	                                         RequestVerificationService::MessageType::FailedProgressOfExecution,
	                                         RequestVerificationService::MessageType::FailedRoutingReport,
	                                         RequestVerificationService::MessageType::FailedStartOfExecution,
	                                         RequestVerificationService::MessageType::SuccessfulAcceptanceReport,
	                                         RequestVerificationService::MessageType::SuccessfulCompletionOfExecution,
	                                         RequestVerificationService::MessageType::SuccessfulProgressOfExecution,
	                                         RequestVerificationService::MessageType::SuccessfulStartOfExecution};

	etl::vector<uint8_t, 15> st03Messages = {
	    HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,
	    HousekeepingService::MessageType::GenerateOneShotHousekeepingReport,
	    HousekeepingService::MessageType::HousekeepingParametersReport,
	    HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
	    HousekeepingService::MessageType::HousekeepingStructuresReport};

	etl::vector<uint8_t, 15> st04Messages = {
	    ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport,
	    ParameterStatisticsService::MessageType::ParameterStatisticsReport,
	};

	etl::vector<uint8_t, 15> st05Messages = {EventReportService::MessageType::HighSeverityAnomalyReport,
	                                         EventReportService::MessageType::DisabledListEventReport,
	                                         EventReportService::MessageType::InformativeEventReport,
	                                         EventReportService::MessageType::LowSeverityAnomalyReport,
	                                         EventReportService::MessageType::MediumSeverityAnomalyReport};

	etl::vector<uint8_t, 15> st06Messages = {MemoryManagementService::MessageType::CheckRawMemoryDataReport,
	                                         MemoryManagementService::MessageType::DumpRawMemoryDataReport};

	etl::vector<uint8_t, 15> st11Messages = {TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport};

	etl::vector<uint8_t, 15> st13Messages = {LargePacketTransferService::MessageType::FirstDownlinkPartReport,
	                                         LargePacketTransferService::MessageType::InternalDownlinkPartReport,
	                                         LargePacketTransferService::MessageType::LastDownlinkPartReport};

	etl::vector<uint8_t, 15> st17Messages = {TestService::MessageType::AreYouAliveTestReport,
	                                         TestService::MessageType::OnBoardConnectionTestReport};

	etl::vector<uint8_t, 15> st19Messages = {EventActionService::MessageType::EventActionStatusReport};

	etl::vector<uint8_t, 15> st20Messages = {ParameterService::MessageType::ParameterValuesReport};

	etl::map<uint8_t, etl::vector<uint8_t, 15>, 100> messagesOfService = {
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
};

#endif
