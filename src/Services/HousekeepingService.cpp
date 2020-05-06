#include "ECSS_Configuration.hpp"
#ifdef SERVICE_HOUSEKEEPING
#include "Services/HousekeepingService.hpp"
#include "ServicePool.hpp"

void HousekeepingService::createHousekeepingStructure(Message& message) {
	message.assertTC(3, 1);

	HousekeepingIdType housekeepingId = message.readUint8();

	if (housekeepingStructureList.full()) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::HousekeepingMapFull);
	} else if (housekeepingStructureList.find(housekeepingId) != housekeepingStructureList.end()) {
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::UsedHousekeepingStructureId);
	} else {
		HousekeepingReportStructure reportStruct; // value of the map
		reportStruct.collectionInterval = message.readUint32(); // assign the collection interval

		ParamIdType numParamIDs = message.readUint8();
		for (uint16_t i = 0; i < numParamIDs; ++i) {
			reportStruct.paramId.push_back(message.readUint16()); // collect the param IDs
		}

		housekeepingStructureList.insert(std::make_pair(housekeepingId, reportStruct)); // insert the requested pair
	}
}

void HousekeepingService::deleteHousekeepingStructure(Message& message) {
	message.assertTC(3, 3);

	uint8_t idCount = message.readUint8();
	for (uint8_t i = 0; i < idCount; ++i) {
		auto it = housekeepingStructureList.find(message.readUint8()); // find the ID
		if (it != housekeepingStructureList.end()) {
			housekeepingStructureList.erase(it);
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::KeyNotFound);
		}
	}
}

void HousekeepingService::enablePeriodicParamReports(Message& message) {
	// Check if the packet is correct
	message.assertTC(3, 5);

	uint8_t idCount = message.readUint8();
	for (uint8_t i = 0; i < idCount; i++) {
		auto it = housekeepingStructureList.find(message.readUint8());
		if (it != housekeepingStructureList.end()) {
			it->second.isPeriodic = true;
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::KeyNotFound);
		}
	}
}

void HousekeepingService::disablePeriodicParamReports(Message& message) {
	message.assertTC(3, 6);

	uint8_t idCount = message.readUint8();
	for (uint8_t i = 0; i < idCount; ++i) {
		auto it = housekeepingStructureList.find(message.readUint8());
		if (it != housekeepingStructureList.end()) {
			it->second.isPeriodic = false;
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::KeyNotFound);
		}
	}
}

void HousekeepingService::checkAndSendHousekeepingReports(TimeAndDate time) {
	uint32_t currSeconds = TimeHelper::utcToSeconds(time);

	for (auto it = housekeepingStructureList.begin(); it != housekeepingStructureList.end(); ++it) {
		uint32_t diffTime = currSeconds - it->second.timestamp;
		if (it->second.isPeriodic && (diffTime >= it->second.collectionInterval)) {

			it->second.timestamp = currSeconds;

			Message report = createTM(25); // create TM[3.25]
			report.appendByte(it->first); // append housekeeping structure ID

			for (auto i : it->second.paramId) {
				if (Services.parameterManagement.isParamId(i)) {
					report.appendString(Services.parameterManagement.returnParamValue(i));
				} else {
					ErrorHandler::reportError(report, ErrorHandler::ExecutionStartErrorType::KeyNotFound);
				}
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
		case 5: enablePeriodicParamReports(message); // TC[3,5]
			break;
		case 6: disablePeriodicParamReports(message); // TC[3,6]
			break;
		default: ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}

#endif