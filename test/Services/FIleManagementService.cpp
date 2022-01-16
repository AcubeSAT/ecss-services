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

    String<64> nullString = "@";

    // Good scenario
    Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<64> repo1 = "test1";
    String<64> file1 = "test2";
    message.appendString(repo1);
    message.appendString(nullString);
    message.appendString(file1);
    message.appendString(nullString);
    message.appendUint32(100);

    MessageParser::execute(message);
    CHECK(ServiceTests::countErrors() == 0);


    // Repository's path string is too large
    Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<1024> repo2 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
    String<64> file2 = "test2";
    message2.appendString(repo2);
    message2.appendString(nullString);
    message2.appendString(file2);
    message2.appendString(nullString);
    message2.appendUint32(100);

    MessageParser::execute(message2);
    CHECK(ServiceTests::countErrors() == 1);
    CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

    // File' name string is too large
    Message message3(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<1024> file3 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
    String<64> repo3 = "test2";
    message3.appendString(repo3);
    message3.appendString(nullString);
    message3.appendString(file3);
    message3.appendString(nullString);
    message3.appendUint32(100);

    MessageParser::execute(message3);
    CHECK(ServiceTests::countErrors() == 2);
    CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

    // File's size is too big
    Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<64> repo4 = "test1";
    String<64> file4 = "test2";
    message4.appendString(repo4);
    message4.appendString(nullString);
    message4.appendString(file4);
    message4.appendString(nullString);
    message4.appendUint32(MAX_FILE_SIZE_BYTES + 10);

    MessageParser::execute(message4);
    CHECK(ServiceTests::countErrors() == 3);
    CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfFileIsOutOfBounds));

    // Repository name has a wildcard
    Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<64> repo5 = "test1*";
    String<64> file5 = "test2";
    message5.appendString(repo5);
    message5.appendString(nullString);
    message5.appendString(file5);
    message5.appendString(nullString);
    message5.appendUint32(1000);

    MessageParser::execute(message5);
    CHECK(ServiceTests::countErrors() == 4);
    CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

    // The repository path leads to a file instead of a repository
    Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<64> repo6 = "test1";
    String<64> file6 = "test2";
    message6.appendString(repo6);
    message6.appendString(nullString);
    message6.appendString(file6);
    message6.appendString(nullString);
    message6.appendUint32(1000);

    MessageParser::execute(message6);
    CHECK(ServiceTests::countErrors() == 5);
    CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));

    // LittleFs returned an unexpected object type
    Message message7(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<64> repo7 = "test1";
    String<64> file7 = "test2";
    message7.appendString(repo7);
    message7.appendString(nullString);
    message7.appendString(file7);
    message7.appendString(nullString);
    message7.appendUint32(1000);

    MessageParser::execute(message7);
    CHECK(ServiceTests::countErrors() == 6);
    CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedFileType));

    // LittleFs function lfs_stat returned with error
    Message message8(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<64> repo8 = "test1";
    String<64> file8 = "test2";
    message8.appendString(repo8);
    message8.appendString(nullString);
    message8.appendString(file8);
    message8.appendString(nullString);
    message8.appendUint32(1000);

    MessageParser::execute(message8);
    CHECK(ServiceTests::countErrors() == 7);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsStatFailed));

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
