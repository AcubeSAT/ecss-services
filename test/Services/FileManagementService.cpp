#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/FileManagementService.hpp>
#include <catch2/catch_all.hpp>
#include <etl/String.hpp>
#include <filesystem>
#include <fstream>
#include "ServiceTests.hpp"

namespace fs = std::filesystem;

// @todo (#277): Add more tests covering cases where the repository and/or the object path is too long.
// For more information check https://gitlab.com/acubesat/obc/ecss-services/-/merge_requests/80#note_1610840164

TEST_CASE("Create a file TC[23,1]", "[service][st23]") {
	fs::current_path(fs::temp_directory_path());
	fs::create_directories("st23");

	// Good scenario
	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
	String<64> repo1 = "st23";
	String<64> file1 = "create_file_1";
	message.appendOctetString(repo1);
	message.appendOctetString(file1);

	uint32_t maxFileSizeBytes = 100;
	message.appendUint32(maxFileSizeBytes);
	bool isFileLocked = false;
	message.appendBoolean(isFileLocked);

	MessageParser::execute(message);
	CHECK(ServiceTests::countErrors() == 0);
	CHECK(fs::exists("st23/create_file_1"));

	// Max file size is too big
	Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<64> repo4 = "st23";
	String<64> file4 = "file2";
	message4.appendOctetString(repo4);
	message4.appendOctetString(file4);

	message4.appendUint32(FileManagementService::MaxPossibleFileSizeBytes + 10);
	message4.appendBoolean(isFileLocked);

	MessageParser::execute(message4);
	CHECK(ServiceTests::countErrors() == 1);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfFileIsOutOfBounds));

	// Repository name has a wildcard
	Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<64> repo5 = "test1*";
	String<64> file5 = "test2";
	message5.appendOctetString(repo5);
	message5.appendOctetString(file5);

	message5.appendUint32(maxFileSizeBytes);
	message5.appendBoolean(isFileLocked);

	MessageParser::execute(message5);
	CHECK(ServiceTests::countErrors() == 2);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// The repository path leads to a file instead of a directory
	std::ofstream file(fs::temp_directory_path() / "st23/create_file_2");
	file.close();
	Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<64> repo6 = "st23/create_file_2";
	String<64> file6 = "test3";
	message6.appendOctetString(repo6);
	message6.appendOctetString(file6);

	message6.appendUint32(maxFileSizeBytes);
	message6.appendBoolean(isFileLocked);

	MessageParser::execute(message6);
	CHECK(ServiceTests::countErrors() == 3);
	CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));

	// File name has a wildcard
	Message message10(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                  0);
	String<1024> repo10 = "test1";
	String<1024> file10 = "test2*";
	message10.appendOctetString(repo10);
	message10.appendOctetString(file10);
	message10.appendUint32(maxFileSizeBytes);

	MessageParser::execute(message10);
	CHECK(ServiceTests::countErrors() == 4);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// File already exists
	Message message11(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                  0);
	String<1024> repo11 = "st23";
	String<1024> file11 = "create_file_1";
	message11.appendOctetString(repo11);
	message11.appendOctetString(file11);
	message11.appendUint32(maxFileSizeBytes);

	MessageParser::execute(message11);
	CHECK(ServiceTests::countErrors() == 5);
	CHECK(ServiceTests::thrownError(ErrorHandler::FileAlreadyExists));

	fs::remove_all(fs::temp_directory_path() / "st23");
}

TEST_CASE("Delete a file TC[23,2]", "[service][st23]") {
	fs::current_path(fs::temp_directory_path());
	fs::create_directories("st23");
	std::ofstream fileToRemove(fs::temp_directory_path() / "st23/file_to_remove_1");
	fileToRemove.close();

	// Good scenario
	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
	String<64> repo1 = "st23";
	String<64> file1 = "file_to_remove_1";
	message.appendOctetString(repo1);
	message.appendOctetString(file1);

	MessageParser::execute(message);
	CHECK(ServiceTests::countErrors() == 0);
	CHECK(fs::exists("st23/file_to_remove_1") == false);

	// Repository name has a wildcard
	Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<64> repo4 = "test1*";
	String<64> file4 = "test2";
	message4.appendOctetString(repo4);
	message4.appendOctetString(file4);

	MessageParser::execute(message4);
	CHECK(ServiceTests::countErrors() == 1);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// File name has a wildcard
	Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<1024> repo5 = "test1";
	String<1024> file5 = "test2*";
	message5.appendOctetString(repo5);
	message5.appendOctetString(file5);

	MessageParser::execute(message5);
	CHECK(ServiceTests::countErrors() == 2);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	std::ofstream fileAsRepository(fs::temp_directory_path() / "st23/file_to_remove_2");
	fileAsRepository.close();
	// Object's repository is a file, so it cannot be removed with this TC
	Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<64> repo9 = "st23/file_to_remove_2";
	String<64> file9 = "test4";
	message9.appendOctetString(repo9);
	message9.appendOctetString(file9);

	MessageParser::execute(message9);
	CHECK(ServiceTests::countErrors() == 3);
	CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));

	fs::remove_all(fs::temp_directory_path() / "st23");
}

TEST_CASE("Report attributes of a file TC[23,3]", "[service][st23]") {
	fs::current_path(fs::temp_directory_path());
	fs::create_directory("st23");
	std::ofstream file(fs::temp_directory_path() / "st23/file_attribute_1");
	String<ECSSMaxStringSize> fileContents = "Hello, world!";
	file << fileContents.data();
	file.close();
	CHECK(fs::exists("st23/file_attribute_1"));

	// Good scenario
	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                Message::TC, 0);
	String<64> repo1 = "st23";
	String<64> file1 = "file_attribute_1";
	message.appendOctetString(repo1);
	message.appendOctetString(file1);

	MessageParser::execute(message);
	CHECK(ServiceTests::countErrors() == 0);

	// Repository name has a wildcard
	Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<64> repo4 = "test1*";
	String<64> file4 = "test2";
	message4.appendOctetString(repo4);
	message4.appendOctetString(file4);

	MessageParser::execute(message4);
	CHECK(ServiceTests::countErrors() == 1);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// File name has a wildcard
	Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<1024> repo5 = "test1";
	String<1024> file5 = "test2*";
	message5.appendOctetString(repo5);
	message5.appendOctetString(file5);

	MessageParser::execute(message5);
	CHECK(ServiceTests::countErrors() == 2);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// The object is not existent
	Message message7(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<64> repo7 = "st23";
	String<64> file7 = "missing_file_1";
	message7.appendOctetString(repo7);
	message7.appendOctetString(file7);

	MessageParser::execute(message7);
	CHECK(ServiceTests::countErrors() == 3);
	CHECK(ServiceTests::thrownError(ErrorHandler::ObjectDoesNotExist));

	// The object's type a directory, not a file
	fs::create_directories("st23/directory_1");
	Message message8(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<64> repo8 = "st23";
	String<64> file8 = "directory_1";
	message8.appendOctetString(repo8);
	message8.appendOctetString(file8);

	MessageParser::execute(message8);
	CHECK(ServiceTests::countErrors() == 4);
	CHECK(ServiceTests::thrownError(ErrorHandler::AttemptedReportAttributesOnDirectory));

	fs::remove_all(fs::temp_directory_path() / "st23");
}

TEST_CASE("File attributes report TM[23,4]", "[service][st23]") {
	fs::current_path(fs::temp_directory_path());
	fs::create_directory("st23");
	std::ofstream file(fs::temp_directory_path() / "st23/report");
	String<ECSSMaxStringSize> fileContents = "Hello, world!";
	file << fileContents.data();
	file.close();

	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                Message::TC, 0);
	String<64> repo1 = "st23";
	String<64> file1 = "report";
	message.appendOctetString(repo1);
	message.appendOctetString(file1);

	MessageParser::execute(message);
	CHECK(ServiceTests::countErrors() == 0);
	REQUIRE(ServiceTests::hasOneMessage());

	// Checking the contents of the report
	Message report = ServiceTests::get(0);

	CHECK(report.readUint16() == 4);
	CHECK(report.readByte() == 's');
	CHECK(report.readByte() == 't');
	CHECK(report.readByte() == '2');
	CHECK(report.readByte() == '3');
	CHECK(report.readUint16() == 6);
	CHECK(report.readByte() == 'r');
	CHECK(report.readByte() == 'e');
	CHECK(report.readByte() == 'p');
	CHECK(report.readByte() == 'o');
	CHECK(report.readByte() == 'r');
	CHECK(report.readByte() == 't');
	CHECK(report.readSint32() == fileContents.size());
	CHECK(report.readBoolean() == false);

	fs::remove_all(fs::temp_directory_path() / "st23");
}
