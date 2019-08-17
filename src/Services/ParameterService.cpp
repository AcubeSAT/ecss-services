#include "Services/ParameterService.hpp"
#include "Services/Parameter.hpp"

ParameterService::ParameterService() {
	// test addings
//	addNewParameter(3, 14);
//	addNewParameter(3, 14);
}

bool ParameterService::addNewParameter(uint8_t ptc, uint8_t pfc, uint32_t initialValue, UpdatePtr ptr) {
	Parameter param = Parameter(ptc, pfc, initialValue, ptr);
	try {
		// second element of the returned std::pair is whether the given item was inserted or not
		return paramsList.insert(std::make_pair(paramsList.size() + 1, param)).second;
	}
	catch(etl::map_full) {
		return false;
	}
}

void ParameterService::reportParameterIds(Message& paramIds) {
	etl::vector<std::pair<uint16_t, ValueType>, MAX_PARAMS> validParamValues;
	paramIds.assertTC(20, 1);
	Message reqParam(20, 2, Message::TM, 1); // empty TM[20, 2] parameter report message

	paramIds.resetRead(); // since we're passing a reference, the reading position shall be reset
	// to its default before any read operations (to ensure the correct data is being read)

	// assertion: correct message, packet and service type (at failure throws an
	// InternalError::UnacceptableMessage)
	ErrorHandler::assertRequest(paramIds.packetType == Message::TC, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.messageType == 1, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(paramIds.serviceType == 20, paramIds,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = paramIds.readUint16();  // number of parameter IDs carried in the message
	uint16_t validIds = 0;

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = paramIds.readUint16();
		try {
			std::pair<ValueType, uint16_t> p = std::make_pair(paramsList.at(currId).getCurrentValue(), i);
			validParamValues.push_back(p);
			validIds++;
		}
		catch (etl::map_out_of_bounds) {
			ErrorHandler::reportError(paramIds, ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
			continue; // generate failed start of execution notification & ignore
		}
	}

	reqParam.appendUint16(validIds);

	for (auto i: validParamValues) {
		reqParam.appendUint16(i.first);
		reqParam.appendUint32(i.second);
	}

//	reqParam.appendUint16(numOfValidIds(paramIds)); // include the number of valid IDs
//
//	for (uint16_t i = 0; i < numOfIds; i++) {
//		uint16_t currId = paramIds.readUint16(); // current ID to be appended
//		try {
//			Parameter foundParam = paramsList.at(currId);
//			reqParam.appendUint16(currId);
//			uint32_t x = foundParam.getCurrentValue();
//			reqParam.appendUint32(foundParam.getCurrentValue());
//		}
//		catch (etl::map_out_of_bounds) {
//
//		}
//	}

	storeMessage(reqParam);
}

void ParameterService::setParameterIds(Message& newParamValues) {
	newParamValues.assertTC(20, 3);

	// assertion: correct message, packet and service type (at failure throws an
	// InternalError::UnacceptablePacket which gets logged)

	ErrorHandler::assertRequest(newParamValues.packetType == Message::TC, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParamValues.messageType == 3, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);
	ErrorHandler::assertRequest(newParamValues.serviceType == 20, newParamValues,
	                            ErrorHandler::AcceptanceErrorType::UnacceptableMessage);

	uint16_t numOfIds = newParamValues.readUint16(); // get number of ID's contained in the message

	for (uint16_t i = 0; i < numOfIds; i++) {
		uint16_t currId = newParamValues.readUint16();

		if (paramsList.find(currId) != paramsList.end()) {
			paramsList[currId].setCurrentValue(newParamValues.readUint32()); // TODO: add a check here with the new
			// flag functionality
		} else {
			ErrorHandler::reportError(newParamValues,
			                          ErrorHandler::ExecutionStartErrorType::UnknownExecutionStartError);
			continue; // generate failed start of execution notification & ignore
		}
	}
}

void ParameterService::execute(Message& message) {
	switch (message.messageType) {
		case 1:
			reportParameterIds(message); // TC[20,1]
			break;
		case 3:
			setParameterIds(message); // TC[20,3]
			break;
		default:
			ErrorHandler::reportInternalError(ErrorHandler::OtherMessageType);
	}
}
