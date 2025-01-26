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
	const ServiceDefinitionsVector& getST01Reports() {
		static const ServiceDefinitionsVector reports = {
			RequestVerificationService::MessageType::FailedAcceptanceReport,
			RequestVerificationService::MessageType::FailedCompletionOfExecution,
			RequestVerificationService::MessageType::FailedProgressOfExecution,
			RequestVerificationService::MessageType::FailedRoutingReport,
			RequestVerificationService::MessageType::FailedStartOfExecution,
			RequestVerificationService::MessageType::SuccessfulAcceptanceReport,
			RequestVerificationService::MessageType::SuccessfulCompletionOfExecution,
			RequestVerificationService::MessageType::SuccessfulProgressOfExecution,
			RequestVerificationService::MessageType::SuccessfulStartOfExecution
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST03Reports() {
		static const ServiceDefinitionsVector reports = {
			HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport,
			HousekeepingService::MessageType::EnablePeriodicHousekeepingParametersReport,
			HousekeepingService::MessageType::GenerateOneShotHousekeepingReport,
			HousekeepingService::MessageType::HousekeepingParametersReport,
			HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
			HousekeepingService::MessageType::HousekeepingStructuresReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST04Reports() {
		static const ServiceDefinitionsVector reports = {
			ParameterStatisticsService::MessageType::ParameterStatisticsDefinitionsReport,
			ParameterStatisticsService::MessageType::ParameterStatisticsReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST05Reports() {
		static const ServiceDefinitionsVector reports = {
			EventReportService::MessageType::HighSeverityAnomalyReport,
			EventReportService::MessageType::DisabledListEventReport,
			EventReportService::MessageType::InformativeEventReport,
			EventReportService::MessageType::LowSeverityAnomalyReport,
			EventReportService::MessageType::MediumSeverityAnomalyReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST06Reports() {
		static const ServiceDefinitionsVector reports = {
			MemoryManagementService::MessageType::CheckRawMemoryDataReport,
			MemoryManagementService::MessageType::DumpRawMemoryDataReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST11Reports() {
		static const ServiceDefinitionsVector reports = {
			TimeBasedSchedulingService::MessageType::TimeBasedScheduledSummaryReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST12Reports() {
		static const ServiceDefinitionsVector reports = {
			OnBoardMonitoringService::MessageType::ParameterMonitoringDefinitionReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST13Reports() {
		static const ServiceDefinitionsVector reports = {
			LargePacketTransferService::MessageType::FirstDownlinkPartReport,
			LargePacketTransferService::MessageType::InternalDownlinkPartReport,
			LargePacketTransferService::MessageType::LastDownlinkPartReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST14Reports() {
		static const ServiceDefinitionsVector reports = {
			RealTimeForwardingControlService::MessageType::AppProcessConfigurationContentReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST15Reports() {
		static const ServiceDefinitionsVector reports = {
			StorageAndRetrievalService::MessageType::ApplicationProcessReport,
			StorageAndRetrievalService::MessageType::PacketStoreConfigurationReport,
			StorageAndRetrievalService::MessageType::PacketStoresStatusReport,
			StorageAndRetrievalService::MessageType::PacketStoreContentSummaryReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST17Reports() {
		static const ServiceDefinitionsVector reports = {
			TestService::MessageType::AreYouAliveTestReport,
			TestService::MessageType::OnBoardConnectionTestReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST19Reports() {
		static const ServiceDefinitionsVector reports = {
			EventActionService::MessageType::EventActionStatusReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST20Reports() {
		static const ServiceDefinitionsVector reports = {
			ParameterService::MessageType::ParameterValuesReport
		};
		return reports;
	}

	const ServiceDefinitionsVector& getST23Reports() {
		static const ServiceDefinitionsVector reports = {
			FileManagementService::MessageType::CreateAttributesReport,
		};
		return reports;
	}

	const etl::map<uint8_t, ServiceDefinitionsVector, ECSSMaxServiceTypeDefinitions>& getMessagesOfService() {
		static const etl::map<uint8_t, ServiceDefinitionsVector, ECSSMaxServiceTypeDefinitions> messages = {
			{RequestVerificationService::ServiceType, getST01Reports()},
			{HousekeepingService::ServiceType, getST03Reports()},
			{ParameterStatisticsService::ServiceType, getST04Reports()},
			{EventReportService::ServiceType, getST05Reports()},
			{MemoryManagementService::ServiceType, getST06Reports()},
			{TimeBasedSchedulingService::ServiceType, getST11Reports()},
			{OnBoardMonitoringService::ServiceType, getST12Reports()},
			{LargePacketTransferService::ServiceType, getST13Reports()},
			{RealTimeForwardingControlService::ServiceType, getST14Reports()},
			{StorageAndRetrievalService::ServiceType, getST15Reports()},
			{TestService::ServiceType, getST17Reports()},
			{EventActionService::ServiceType, getST19Reports()},
			{ParameterService::ServiceType, getST20Reports()},
			{FileManagementService::ServiceType, getST23Reports()},
		};
		return messages;
	}
} // namespace AllReportTypes
