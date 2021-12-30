#include <catch2/catch.hpp>
#include <Services/FileManagementService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <etl/String.hpp>
#include <cstring>
#include <ServicePool.hpp>

//EventActionService& eventActionService = Services.eventAction;
FileManagementService& fileManagementService = Services.fileManagementService;

TEST_CASE("Create a file TC[23,1]", "[service][st23]"){

    Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<64> data1 = "test1";
    String<64> data2 = "test2";
    message.appendString(data1);
    message.appendString(data2);
    message.appendUint8(100);

    MessageParser::execute(message);

}

/*

TEST_CASE("Add event-action definitions TC[19,1]", "[service][st19]") {

    // Add a message that is too large to check for the corresponding error
    Message message(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
    message.appendEnum16(0);
    message.appendEnum16(1);
    message.appendEnum16(1);
    String<128> data = "0123456789012345678901234567890123456789012345678901234567890123456789";
    message.appendString(data);

    MessageParser::execute(message);
    CHECK(ServiceTests::thrownError(ErrorHandler::MessageTooLarge));
    CHECK(ServiceTests::countErrors() == 1);

    // Add an event-action definition to check if the values are inserted correctly
    Message message1(EventActionService::ServiceType, EventActionService::MessageType::AddEventAction, Message::TC, 0);
    message1.appendEnum16(0);
    message1.appendEnum16(2);
    message1.appendEnum16(1);
    data = "01234";
    message1.appendString(data);
    MessageParser::execute(message1);

    CHECK(eventActionService.eventActionDefinitionMap.lower_bound(2)->second.applicationId == 0);
    CHECK(eventActionService.eventActionDefinitionMap.lower_bound(2)->second.eventDefinitionID == 2);
    CHECK(eventActionService.eventActionDefinitionMap.lower_bound(2)->second.enabled == 0);
    CHECK(eventActionService.eventActionDefinitionMap.lower_bound(2)->second.request.compare(data) == 0);
}

*/
