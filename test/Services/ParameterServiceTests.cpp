#include "Services/ParameterService.hpp"
#include "Message.hpp"
#include "Parameters/PlatformParameters.hpp"
#include "ServiceTests.hpp"
#include "catch2/catch_all.hpp"

static void resetParameterValues() {
	PlatformParameters::parameter1.setValue(3);
	PlatformParameters::parameter2.setValue(7);
	PlatformParameters::parameter3.setValue(10);
};

TEST_CASE("Parameter Report Subservice") {
	SECTION("All requested parameters invalid") {
		Message request = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
		                          Message::TC, ApplicationId);
		request.appendUint16(3);
		request.appendUint16(54432);
		request.appendUint16(60000);
		request.appendUint16(65535);

		MessageParser::execute(request);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameter) == 3);
		CHECK(ServiceTests::count() == 4);

		Message report = ServiceTests::get(3);
		CHECK(report.serviceType == ParameterService::ServiceType);
		CHECK(report.messageType == ParameterService::MessageType::ParameterValuesReport);
		CHECK(report.readUint16() == 0); // the message shall be empty

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Some requested parameters invalid") {
		Message request = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
		                          Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(1);
		request.appendUint16(10000);
		request.appendUint16(2);

		MessageParser::execute(request);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::GetNonExistingParameter) == 1);
		CHECK(ServiceTests::count() == 2);

		Message report = ServiceTests::get(1);
		CHECK(report.serviceType == ParameterService::ServiceType);
		CHECK(report.messageType == ParameterService::MessageType::ParameterValuesReport);
		CHECK(report.readUint16() == 2);
		CHECK(report.readUint16() == 1);
		CHECK(report.readUint16() == 7);
		CHECK(report.readUint16() == 2);
		CHECK(report.readUint32() == 10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Parameters are of different types") {
		Message request = Message(ParameterService::ServiceType, ParameterService::MessageType::ReportParameterValues,
		                          Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(0);
		request.appendUint16(1);
		request.appendUint16(2);

		MessageParser::execute(request);

		Message report = ServiceTests::get(0);
		CHECK(report.serviceType == ParameterService::ServiceType);
		CHECK(report.messageType == ParameterService::MessageType::ParameterValuesReport);
		CHECK(report.readUint16() == 3);
		CHECK(report.readUint16() == 0);
		CHECK(report.readUint8() == 3);
		CHECK(report.readUint16() == 1);
		CHECK(report.readUint16() == 7);
		CHECK(report.readUint16() == 2);
		CHECK(report.readUint32() == 10);

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Parameter Setting Subservice") {
	SECTION("All parameter IDs are invalid") {
		Message request =
		    Message(ParameterService::ServiceType, ParameterService::MessageType::SetParameterValues, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(54432);
		request.appendUint16(1);
		request.appendUint16(60000);
		request.appendUint16(1);
		request.appendUint16(65534);
		request.appendUint16(1);

		MessageParser::execute(request);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingParameter) == 1);
		CHECK(ServiceTests::count() == 1);

		CHECK(PlatformParameters::parameter1.getValue() == 3);
		CHECK(PlatformParameters::parameter2.getValue() == 7);
		CHECK(PlatformParameters::parameter3.getValue() == 10);

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("The last parameter ID is invalid") {
		Message request =
		    Message(ParameterService::ServiceType, ParameterService::MessageType::SetParameterValues, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(0);
		request.appendUint8(1);
		request.appendUint16(1);
		request.appendUint16(2);
		request.appendUint16(65534);
		request.appendUint16(1);

		MessageParser::execute(request);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingParameter) == 1);
		CHECK(ServiceTests::count() == 1);

		CHECK(PlatformParameters::parameter1.getValue() == 1);
		CHECK(PlatformParameters::parameter2.getValue() == 2);
		CHECK(PlatformParameters::parameter3.getValue() == 10);

		resetParameterValues();

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("The middle parameter ID is invalid") {
		Message request =
		    Message(ParameterService::ServiceType, ParameterService::MessageType::SetParameterValues, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(0);
		request.appendUint8(1);
		request.appendUint16(65534);
		request.appendUint16(1);
		request.appendUint16(2);
		request.appendUint16(3);

		MessageParser::execute(request);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::SetNonExistingParameter) == 1);
		CHECK(ServiceTests::count() == 1);

		CHECK(PlatformParameters::parameter1.getValue() == 1);
		CHECK(PlatformParameters::parameter2.getValue() == 7);
		CHECK(PlatformParameters::parameter3.getValue() == 10);

		resetParameterValues();

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("All IDs are valid") {
		Message request =
		    Message(ParameterService::ServiceType, ParameterService::MessageType::SetParameterValues, Message::TC, 1);
		request.appendUint16(3);
		request.appendUint16(0);
		request.appendUint8(1);
		request.appendUint16(1);
		request.appendUint16(2);
		request.appendUint16(2);
		request.appendUint32(3);

		MessageParser::execute(request);

		CHECK(PlatformParameters::parameter1.getValue() == 1);
		CHECK(PlatformParameters::parameter2.getValue() == 2);
		CHECK(PlatformParameters::parameter3.getValue() == 3);

		resetParameterValues();

		ServiceTests::reset();
		Services.reset();
	}
}

TEST_CASE("Wrong Messages") {
	SECTION("Wrong Service Type Handling Test for Report") {
		Message falseRequest(62, 1, Message::TM, 1);

		MessageParser::execute(falseRequest);
		CHECK(ServiceTests::thrownError(ErrorHandler::InternalErrorType::OtherMessageType));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Wrong Service Type Handling Test for Set") {
		Message falseRequest(62, 3, Message::TM, 1);

		MessageParser::execute(falseRequest);
		CHECK(ServiceTests::thrownError(ErrorHandler::InternalErrorType::OtherMessageType));

		ServiceTests::reset();
		Services.reset();
	}

	SECTION("Wrong Message Type") {
		Message falseRequest(20, 127, Message::TM, 1);

		MessageParser::execute(falseRequest);
		CHECK(ServiceTests::thrownError(ErrorHandler::InternalErrorType::OtherMessageType));

		ServiceTests::reset();
		Services.reset();
	}
}
