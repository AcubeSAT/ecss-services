#include <iostream>
#include <Logger.hpp>
#include <Time/UTCTimestamp.hpp>
#include <Platform/x86/Helpers/UTCTimestamp.hpp>
#include "Helpers/CRCHelper.hpp"
#include "Services/TestService.hpp"
#include "Services/ParameterService.hpp"
#include "Services/RequestVerificationService.hpp"
#include "Services/MemoryManagementService.hpp"
#include "Services/EventReportService.hpp"
#include "Services/FunctionManagementService.hpp"
#include "Services/EventActionService.hpp"
#include "Services/LargePacketTransferService.hpp"
#include "Services/TimeBasedSchedulingService.hpp"
#include "Services/ParameterStatisticsService.hpp"
#include "Helpers/Statistic.hpp"
#include "Message.hpp"
#include "MessageParser.hpp"
#include "ErrorHandler.hpp"
#include "etl/String.hpp"
#include "ServicePool.hpp"
#include <ctime>

int main() {
	LOG_NOTICE << "ECSS Services test application";

	LOG_NOTICE << "Size of CUC timestamp: " << sizeof(TimeStamp<4, 4>);

	return 0;
}
