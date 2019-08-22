#include "Services/HousekeepingService.hpp"
#include "ServicePool.hpp"

HousekeepingService::HousekeepingService() {
	serviceType = 3;
}

void HousekeepingService::createHousekeepingStructure(Message& message) {
	// Check if the packet is correct
	ErrorHandler::assertRequest(message.packetType == Message::TC, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
	ErrorHandler::assertRequest(message.messageType == 1, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage;
	ErrorHandler::assertRequest(message.serviceType == 3, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);

	HousekeepingId housekeepingId = message.readUint8(); // assign the housekeeping structure ID
	ErrorHandler::assertRequest(housekeepingStructureList.find(housekeepingId) == housekeepingStructureList.end(),
		message, ErrorHandler::ExecutionStartErrorType::UsedHousekeepingStructureId); //  reject request if id exists

	HousekeepingReportStructure reportStruct;

	reportStruct.collectInterval = message.readUint32(); // assign the collection interval

	ParamId numParamIDs = message.readUint8();
	ErrorHandler::assertRequest(numParamIDs < MAX_PARAMS, message,
		ErrorHandler::AcceptanceErrorType::ExceedMaxNumParam);
	for (int i = 0; i < numParamIDs; i++) {
		reportStruct.paramId.push_back(message.readUint16()); // collect the param IDs
	}

	ErrorHandler::assertInternal(not housekeepingStructureList.full(),
		ErrorHandler::InternalErrorType::FunctionMapFull);
	housekeepingStructureList.insert(std::make_pair(housekeepingId, reportStruct)); // insert the requested structure
}

void HousekeepingService::deleteHousekeepingStructure(Message& message) {
	// Check if the packet is correct
	ErrorHandler::assertRequest(message.packetType == Message::TC, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
	ErrorHandler::assertRequest(message.messageType == 3, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
	ErrorHandler::assertRequest(message.serviceType == 3, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
}

void HousekeepingService::enablePeriodParamReports(Message& message) {
	// Check if the packet is correct
	ErrorHandler::assertRequest(message.packetType == Message::TC, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
	ErrorHandler::assertRequest(message.messageType == 5, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
	ErrorHandler::assertRequest(message.serviceType == 3, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
}

void HousekeepingService::disablePeriodParamReports(Message& message) {
	// Check if the packet is correct
	ErrorHandler::assertRequest(message.packetType == Message::TC, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
	ErrorHandler::assertRequest(message.messageType == 6, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
	ErrorHandler::assertRequest(message.serviceType == 3, message,
		ErrorHandler::ExecutionStartErrorType::UnexpectedMessage);
}

void HousekeepingService::paramReport(TimeAndDate time) {
	uint32_t currSeconds = TimeHelper::utcToSeconds(time); // convert UTC date to seconds
	bool oneMessage = true; // only one Message should be generated

	for (auto it = housekeepingStructureList.begin(); it != housekeepingStructureList.end(); it++) {
		uint32_t diffTime = currSeconds - it->second.timestamp; // time diff between current and last time called
		it->second.periodicStatus = true; // enable just for now
		if ((diffTime >= it->second.collectInterval) && it->second.periodicStatus && oneMessage) { // find the
			// structure with the appropriate interval
			oneMessage = false;
			it->second.timestamp = currSeconds; // update the timestamp
			Message report = createTM(25); // create TM[3.25]

			report.appendByte(it->first); // append housekeeping structure ID

			for (auto i : it->second.paramId) {
				auto itParam = Services.parameterManagement.paramsList.find(i); // find the requested paramID
				report.appendWord(itParam->second.getCurrentValue()); // fetch the requested parameters that are
				// already configured from the paramete service
			}
			storeMessage(report);
		}
	}
}

void HousekeepingService::execute(Message& message) {
	switch (message.messageType) {
		case 1: createHousekeepingStructure(message); // TC[3,1]
			break;
		case 3: deleteHousekeepingStructure(message); // TC[3,3]
			break;
		case 5: enablePeriodParamReports(message); // TC[3,5]
			break;
		case 6: disablePeriodParamReports(message); // TC[3,6]
			break;
		default: ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
			break;
	}
}
