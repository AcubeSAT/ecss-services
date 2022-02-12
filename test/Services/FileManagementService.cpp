#include <catch2/catch.hpp>
#include <Services/FileManagementService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <etl/String.hpp>
#include <cstring>
#include <ServicePool.hpp>


//EventActionService& eventActionService = Services.eventAction;
FileManagementService& fileManagementService = Services.fileManagement;

TEST_CASE("Create a file TC[23,1]", "[service][st23]"){

    String<64> nullString = "@";

    // Good scenario
    Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<64> repo1 = "test1/";
    String<64> file1 = "/test2";
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
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsInvalidObjectType));

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

    // Object's path string size is too large
    Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<1024> repo9 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
    String<1024> file9 = "test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2";
    message9.appendString(repo9);
    message9.appendString(nullString);
    message9.appendString(file9);
    message9.appendString(nullString);
    message9.appendUint32(1000);

    MessageParser::execute(message9);
    CHECK(ServiceTests::countErrors() == 8);
    CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

    // File name contains a wildcard
    Message message10(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<1024> repo10 = "test1";
    String<1024> file10 = "test2*";
    message10.appendString(repo10);
    message10.appendString(nullString);
    message10.appendString(file10);
    message10.appendString(nullString);
    message10.appendUint32(1000);

    MessageParser::execute(message10);
    CHECK(ServiceTests::countErrors() == 9);
    CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

    // File already exists
    Message message11(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<1024> repo11 = "test1";
    String<1024> file11 = "test2";
    message11.appendString(repo11);
    message11.appendString(nullString);
    message11.appendString(file11);
    message11.appendString(nullString);
    message11.appendUint32(1000);

    MessageParser::execute(message11);
    CHECK(ServiceTests::countErrors() == 10);
    CHECK(ServiceTests::thrownError(ErrorHandler::FileAlreadyExists));

    // LittleFs generated an error during the creation of the file
    Message message12(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<1024> repo12 = "test1";
    String<1024> file12 = "test2";
    message12.appendString(repo12);
    message12.appendString(nullString);
    message12.appendString(file12);
    message12.appendString(nullString);
    message12.appendUint32(1000);

    MessageParser::execute(message12);
    CHECK(ServiceTests::countErrors() == 11);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsFileOpenFailed));

    // LittleFs failed to close the file after creation, in order to synchronize the file system
    Message message13(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
    String<1024> repo13 = "test1";
    String<1024> file13 = "test2";
    message13.appendString(repo13);
    message13.appendString(nullString);
    message13.appendString(file13);
    message13.appendString(nullString);
    message13.appendUint32(1000);

    MessageParser::execute(message13);
    CHECK(ServiceTests::countErrors() == 12);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsFileCloseFailed));
}

TEST_CASE("Delete a file TC[23,2]", "[service][st23]") {

    String<64> nullString = "@";

    // Good scenario
    Message message(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
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
    Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
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
    Message message3(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
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

    // Repository name has a wildcard
    Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
    String<64> repo4 = "test1*";
    String<64> file4 = "test2";
    message4.appendString(repo4);
    message4.appendString(nullString);
    message4.appendString(file4);
    message4.appendString(nullString);
    message4.appendUint32(1000);

    MessageParser::execute(message4);
    CHECK(ServiceTests::countErrors() == 3);
    CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

    // File name contains a wildcard
    Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
    String<1024> repo5 = "test1";
    String<1024> file5 = "test2*";
    message5.appendString(repo5);
    message5.appendString(nullString);
    message5.appendString(file5);
    message5.appendString(nullString);
    message5.appendUint32(1000);

    MessageParser::execute(message5);
    CHECK(ServiceTests::countErrors() == 4);
    CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

    // Object's path string size is too large
    Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
    String<1024> repo6 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
    String<1024> file6 = "test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2";
    message6.appendString(repo6);
    message6.appendString(nullString);
    message6.appendString(file6);
    message6.appendString(nullString);
    message6.appendUint32(1000);

    MessageParser::execute(message6);
    CHECK(ServiceTests::countErrors() == 5);
    CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

    // LittleFs returned an unexpected object type
    Message message7(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
    String<64> repo7 = "test1";
    String<64> file7 = "test2";
    message7.appendString(repo7);
    message7.appendString(nullString);
    message7.appendString(file7);
    message7.appendString(nullString);
    message7.appendUint32(1000);

    MessageParser::execute(message7);
    CHECK(ServiceTests::countErrors() == 6);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsInvalidObjectType));

    // LittleFs function lfs_stat returned with error
    Message message8(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
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

    // Object's repository is a file, so it cannot be removed with this TC
    Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
    String<64> repo9 = "test1";
    String<64> file9 = "test2";
    message9.appendString(repo9);
    message9.appendString(nullString);
    message9.appendString(file9);
    message9.appendString(nullString);
    message9.appendUint32(1000);

    MessageParser::execute(message9);
    CHECK(ServiceTests::countErrors() == 8);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsInvalidObjectType));

    // lfs_remove faild
    Message message10(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
    String<64> repo10 = "test1";
    String<64> file10 = "test2";
    message10.appendString(repo10);
    message10.appendString(nullString);
    message10.appendString(file10);
    message10.appendString(nullString);
    message10.appendUint32(1000);

    MessageParser::execute(message10);
    CHECK(ServiceTests::countErrors() == 9);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsRemoveFailed));
}

TEST_CASE("Report attributes of a file TC[23,3]", "[service][st23]") {

    String<64> nullString = "@";

    // Good scenario
    Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
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
    Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
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
    Message message3(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
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

    // Repository name has a wildcard
    Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
    String<64> repo4 = "test1*";
    String<64> file4 = "test2";
    message4.appendString(repo4);
    message4.appendString(nullString);
    message4.appendString(file4);
    message4.appendString(nullString);
    message4.appendUint32(1000);

    MessageParser::execute(message4);
    CHECK(ServiceTests::countErrors() == 3);
    CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

    // File name contains a wildcard
    Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
    String<1024> repo5 = "test1";
    String<1024> file5 = "test2*";
    message5.appendString(repo5);
    message5.appendString(nullString);
    message5.appendString(file5);
    message5.appendString(nullString);
    message5.appendUint32(1000);

    MessageParser::execute(message5);
    CHECK(ServiceTests::countErrors() == 4);
    CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

    // Object's path string size is too large
    Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
    String<1024> repo6 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
    String<1024> file6 = "test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2";
    message6.appendString(repo6);
    message6.appendString(nullString);
    message6.appendString(file6);
    message6.appendString(nullString);
    message6.appendUint32(1000);

    MessageParser::execute(message6);
    CHECK(ServiceTests::countErrors() == 5);
    CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

    // The object's type is invalid
    Message message7(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
    String<64> repo7 = "test1";
    String<64> file7 = "test2";
    message7.appendString(repo7);
    message7.appendString(nullString);
    message7.appendString(file7);
    message7.appendString(nullString);
    message7.appendUint32(100);

    MessageParser::execute(message7);
    CHECK(ServiceTests::countErrors() == 6);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsInvalidObjectType));

    // The object's type a directory, not a file
    Message message8(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
    String<64> repo8 = "test1";
    String<64> file8 = "test2";
    message8.appendString(repo8);
    message8.appendString(nullString);
    message8.appendString(file8);
    message8.appendString(nullString);
    message8.appendUint32(100);

    MessageParser::execute(message8);
    CHECK(ServiceTests::countErrors() == 7);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsInvalidObjectType));

    // lfs_stat (used to retrieve object's information) failed
    Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes, Message::TC, 0);
    String<64> repo9 = "test1";
    String<64> file9 = "test2";
    message9.appendString(repo9);
    message9.appendString(nullString);
    message9.appendString(file9);
    message9.appendString(nullString);
    message9.appendUint32(100);

    MessageParser::execute(message9);
    CHECK(ServiceTests::countErrors() == 8);
    CHECK(ServiceTests::thrownError(ErrorHandler::LittleFsStatFailed));
}

TEST_CASE("File attributes report TM[23,4]", "[service][st23]") {

    String<64> nullString = "@";

    Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
                    Message::TC, 0);
    String<64> repo1 = "test1";
    String<64> file1 = "test2";
    message.appendString(repo1);
    message.appendString(nullString);
    message.appendString(file1);
    message.appendString(nullString);

    MessageParser::execute(message);
    CHECK(ServiceTests::countErrors() == 0);
    REQUIRE(ServiceTests::hasOneMessage());

    // Checking the contents of the report
    Message report = ServiceTests::get(0);

    CHECK(report.readByte() == 't');
    CHECK(report.readByte() == 'e');
    CHECK(report.readByte() == 's');
    CHECK(report.readByte() == 't');
    CHECK(report.readByte() == '1');
    CHECK(report.readByte() == '@');
    CHECK(report.readByte() == 't');
    CHECK(report.readByte() == 'e');
    CHECK(report.readByte() == 's');
    CHECK(report.readByte() == 't');
    CHECK(report.readByte() == '2');
    CHECK(report.readSint32() == 100);

}
