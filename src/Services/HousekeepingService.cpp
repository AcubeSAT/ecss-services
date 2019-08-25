#include "Services/HousekeepingService.hpp"
#include "ServicePool.hpp"

HousekeepingService::HousekeepingService() {
	serviceType = 3;
}

void HousekeepingService::createHousekeepingStructure(Message& message) {
	// Check if the packet is correct
	message.assertTC(3, 1);

	HousekeepingIdType housekeepingId = message.readUint8(); // assign the housekeeping structure ID (key)

	if (housekeepingStructureList.full()) {
		// map is full
		ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::HousekeepingMapFull);
	} else if (housekeepingStructureList.find(housekeepingId) != housekeepingStructureList.end()) {
		// reject requests with IDs that are already assigned
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
	// Check if the packet is correct
	message.assertTC(3, 3);

	uint8_t idCount = message.readUint8(); // find the number of the structures that will be deleted
	for (uint8_t i = 0; i < idCount; ++i) {
		auto it = housekeepingStructureList.find(message.readUint8()); // find the ID
		if (it != housekeepingStructureList.end()) {
			housekeepingStructureList.erase(it); // delete the housekeeping structure
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::Unknownkey);
		}
	}
}

void HousekeepingService::enablePeriodicParamReports(Message& message) {
	// Check if the packet is correct
	message.assertTC(3, 5);

	uint8_t idCount = message.readUint8(); // find the number of the structures that will be enabled
	for (uint8_t i = 0; i < idCount; i++) {
		auto it = housekeepingStructureList.find(message.readUint8()); // find the ID
		if (it != housekeepingStructureList.end()) {
			it->second.isPeriodic = true; // enable the periodic generation of param reports
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::Unknownkey);
		}
	}
}

void HousekeepingService::disablePeriodicParamReports(Message& message) {
	// Check if the packet is correct
	message.assertTC(3, 6);

	uint8_t idCount = message.readUint8(); // find the number of the structures that will be disabled
	for (uint8_t i = 0; i < idCount; ++i) {
		auto it = housekeepingStructureList.find(message.readUint8()); // find the ID
		if (it != housekeepingStructureList.end()) {
			it->second.isPeriodic = false; // disable the periodic generation of param reports
		} else {
			ErrorHandler::reportError(message, ErrorHandler::ExecutionStartErrorType::Unknownkey);
		}
	}
}

void HousekeepingService::checkAndSendHousekeepingReports(TimeAndDate time) {
	uint32_t currSeconds = TimeHelper::utcToSeconds(time); // convert UTC date to seconds

	for (auto it = housekeepingStructureList.begin(); it != housekeepingStructureList.end(); ++it) {
		uint32_t diffTime = currSeconds - it->second.timestamp; // time diff between current and last time called
		if (it->second.isPeriodic && (diffTime >= it->second.collectionInterval)) {
			// the time has passed, now the housekeeping report should be sent

			it->second.timestamp = currSeconds; // update the timestamp

			Message report = createTM(25); // create TM[3.25]
			report.appendByte(it->first); // append housekeeping structure ID

			for (auto i : it->second.paramId) {
				auto itParam = Services.parameterManagement.paramsList.find(i); // find the requested paramID
				if (itParam != Services.parameterManagement.paramsList.end()) {
					report.appendWord(itParam->second.getCurrentValue()); // fetch the requested parameters values
				} else {
					ErrorHandler::reportError(report, ErrorHandler::ExecutionStartErrorType::Unknownkey);
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
