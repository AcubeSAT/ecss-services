#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/FileManagementService.hpp>
#include <catch2/catch_all.hpp>
#include <etl/String.hpp>
#include <filesystem>
#include <fstream>
#include "ServiceTests.hpp"

namespace fs = std::filesystem;

TEST_CASE("Create a file TC[23,1]", "[service][st23]") {
	fs::remove_all(fs::temp_directory_path() / "test1");
	fs::remove_all(fs::temp_directory_path() / "test2");
	fs::current_path(fs::temp_directory_path());
	fs::create_directories("test1");

	// Good scenario
	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
	String<64> repo1 = "test1";
	String<64> file1 = "test2";
	message.appendString(repo1);
	message.append(FileManagementService::VariableStringTerminator);
	message.appendString(file1);
	message.append(FileManagementService::VariableStringTerminator);
	uint32_t maxFileSizeBytes = 100;
	message.appendUint32(maxFileSizeBytes);
	bool isFileLocked = false;
	message.appendBoolean(isFileLocked);

	MessageParser::execute(message);
	CHECK(ServiceTests::countErrors() == 0);


	// Repository path string is too large
	Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<1024> repo2 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	String<64> file2 = "test2";
	message2.appendString(repo2);
	message2.append(FileManagementService::VariableStringTerminator);
	message2.appendString(file2);
	message2.append(FileManagementService::VariableStringTerminator);
	message2.appendUint32(maxFileSizeBytes);
	message2.appendBoolean(isFileLocked);

	MessageParser::execute(message2);
	CHECK(ServiceTests::countErrors() == 1);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	// File name string is too large
	Message message3(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<64> repo3 = "test2";
	String<1024> file3 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	message3.appendString(repo3);
	message3.append(FileManagementService::VariableStringTerminator);
	message3.appendString(file3);
	message3.append(FileManagementService::VariableStringTerminator);
	message3.appendUint32(maxFileSizeBytes);
	message3.appendBoolean(isFileLocked);

	MessageParser::execute(message3);
	CHECK(ServiceTests::countErrors() == 2);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	// File's size is too big
	Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<64> repo4 = "test1";
	String<64> file4 = "test2";
	message4.appendString(repo4);
	message4.append(FileManagementService::VariableStringTerminator);
	message4.appendString(file4);
	message4.append(FileManagementService::VariableStringTerminator);
	message4.appendUint32(FileManagementService::MaxPossibleFileSizeBytes + 10);
	message4.appendBoolean(isFileLocked);

	MessageParser::execute(message4);
	CHECK(ServiceTests::countErrors() == 3);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfFileIsOutOfBounds));

	// Repository name has a wildcard
	Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<64> repo5 = "test1*";
	String<64> file5 = "test2";
	message5.appendString(repo5);
	message5.append(FileManagementService::VariableStringTerminator);
	message5.appendString(file5);
	message5.append(FileManagementService::VariableStringTerminator);
	message5.appendUint32(maxFileSizeBytes);
	message5.appendBoolean(isFileLocked);

	MessageParser::execute(message5);
	CHECK(ServiceTests::countErrors() == 4);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// The repository path leads to a file instead of a repository
	std::ofstream file(fs::temp_directory_path() / "test2");
	file.close();
	Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<64> repo6 = "test2";
	String<64> file6 = "test3";
	message6.appendString(repo6);
	message6.append(FileManagementService::VariableStringTerminator);
	message6.appendString(file6);
	message6.append(FileManagementService::VariableStringTerminator);
	message6.appendUint32(maxFileSizeBytes);
	message6.appendBoolean(isFileLocked);

	MessageParser::execute(message6);
	CHECK(ServiceTests::countErrors() == 5);
	CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));

	// Object's path string size is too large
	Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                 0);
	String<1024> repo9 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	String<1024> file9 = "test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2";
	message9.appendString(repo9);
	message9.append(FileManagementService::VariableStringTerminator);
	message9.appendString(file9);
	message9.append(FileManagementService::VariableStringTerminator);
	message9.appendUint32(1000);

	MessageParser::execute(message9);
	CHECK(ServiceTests::countErrors() == 6);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	// File name contains a wildcard
	Message message10(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                  0);
	String<1024> repo10 = "test1";
	String<1024> file10 = "test2*";
	message10.appendString(repo10);
	message10.append(FileManagementService::VariableStringTerminator);
	message10.appendString(file10);
	message10.append(FileManagementService::VariableStringTerminator);
	message10.appendUint32(1000);

	MessageParser::execute(message10);
	CHECK(ServiceTests::countErrors() == 7);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// File already exists
	Message message11(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
	                  0);
	String<1024> repo11 = "test1";
	String<1024> file11 = "test2";
	message11.appendString(repo11);
	message11.append(FileManagementService::VariableStringTerminator);
	message11.appendString(file11);
	message11.append(FileManagementService::VariableStringTerminator);
	message11.appendUint32(1000);

	MessageParser::execute(message11);
	CHECK(ServiceTests::countErrors() == 8);
	CHECK(ServiceTests::thrownError(ErrorHandler::FileAlreadyExists));

	fs::remove_all(fs::temp_directory_path() / "test1");
}

TEST_CASE("Delete a file TC[23,2]", "[service][st23]") {
	fs::current_path(fs::temp_directory_path());
	fs::create_directories("test1");
	// The repository path leads to a file instead of a repository
	std::ofstream fileToRemove(fs::temp_directory_path() / "test1/test2");
	fileToRemove.close();

	// Good scenario
	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
	String<64> repo1 = "test1";
	String<64> file1 = "test2";
	message.appendString(repo1);
	message.append(FileManagementService::VariableStringTerminator);
	message.appendString(file1);
	message.append(FileManagementService::VariableStringTerminator);
	message.appendUint32(100);

	MessageParser::execute(message);
	CHECK(ServiceTests::countErrors() == 0);

	// Repository's path string is too large
	Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<1024> repo2 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	String<64> file2 = "test2";
	message2.appendString(repo2);
	message2.append(FileManagementService::VariableStringTerminator);
	message2.appendString(file2);
	message2.append(FileManagementService::VariableStringTerminator);
	message2.appendUint32(100);

	MessageParser::execute(message2);
	CHECK(ServiceTests::countErrors() == 1);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	// File' name string is too large
	Message message3(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<1024> file3 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	String<64> repo3 = "test2";
	message3.appendString(repo3);
	message3.append(FileManagementService::VariableStringTerminator);
	message3.appendString(file3);
	message3.append(FileManagementService::VariableStringTerminator);
	message3.appendUint32(100);

	MessageParser::execute(message3);
	CHECK(ServiceTests::countErrors() == 2);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	// Repository name has a wildcard
	Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<64> repo4 = "test1*";
	String<64> file4 = "test2";
	message4.appendString(repo4);
	message4.append(FileManagementService::VariableStringTerminator);
	message4.appendString(file4);
	message4.append(FileManagementService::VariableStringTerminator);
	message4.appendUint32(1000);

	MessageParser::execute(message4);
	CHECK(ServiceTests::countErrors() == 3);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// File name contains a wildcard
	Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<1024> repo5 = "test1";
	String<1024> file5 = "test2*";
	message5.appendString(repo5);
	message5.append(FileManagementService::VariableStringTerminator);
	message5.appendString(file5);
	message5.append(FileManagementService::VariableStringTerminator);
	message5.appendUint32(1000);

	MessageParser::execute(message5);
	CHECK(ServiceTests::countErrors() == 4);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// Object's path string size is too large
	Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<1024> repo6 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	String<1024> file6 = "test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2";
	message6.appendString(repo6);
	message6.append(FileManagementService::VariableStringTerminator);
	message6.appendString(file6);
	message6.append(FileManagementService::VariableStringTerminator);
	message6.appendUint32(1000);

	MessageParser::execute(message6);
	CHECK(ServiceTests::countErrors() == 5);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	std::ofstream fileAsRepository(fs::temp_directory_path() / "test3");
	fileAsRepository.close();
	// Object's repository is a file, so it cannot be removed with this TC
	Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
	                 0);
	String<64> repo9 = "test3";
	String<64> file9 = "test4";
	message9.appendString(repo9);
	message9.append(FileManagementService::VariableStringTerminator);
	message9.appendString(file9);
	message9.append(FileManagementService::VariableStringTerminator);
	message9.appendUint32(1000);

	MessageParser::execute(message9);
	CHECK(ServiceTests::countErrors() == 6);
	CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));

	fs::remove_all(fs::temp_directory_path() / "test1");
}

TEST_CASE("Report attributes of a file TC[23,3]", "[service][st23]") {
	fs::current_path(fs::temp_directory_path());
	fs::create_directory("test1");
	std::ofstream file(fs::temp_directory_path() / "test1/test2");
	String<ECSSMaxStringSize> fileContents = "Hello, world!";
	file << fileContents.data();
	file.close();

	// Good scenario
	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                Message::TC, 0);
	String<64> repo1 = "test1";
	String<64> file1 = "test2";
	message.appendString(repo1);
	message.append(FileManagementService::VariableStringTerminator);
	message.appendString(file1);
	message.append(FileManagementService::VariableStringTerminator);

	MessageParser::execute(message);
	CHECK(ServiceTests::countErrors() == 0);

	// Repository's path string is too large
	Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<1024> repo2 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	String<64> file2 = "test2";
	message2.appendString(repo2);
	message2.append(FileManagementService::VariableStringTerminator);
	message2.appendString(file2);
	message2.append(FileManagementService::VariableStringTerminator);

	MessageParser::execute(message2);
	CHECK(ServiceTests::countErrors() == 1);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	// File' name string is too large
	Message message3(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<64> repo3 = "test2";
	String<1024> file3 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	message3.appendString(repo3);
	message3.append(FileManagementService::VariableStringTerminator);
	message3.appendString(file3);
	message3.append(FileManagementService::VariableStringTerminator);

	MessageParser::execute(message3);
	CHECK(ServiceTests::countErrors() == 2);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	// Repository name has a wildcard
	Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<64> repo4 = "test1*";
	String<64> file4 = "test2";
	message4.appendString(repo4);
	message4.append(FileManagementService::VariableStringTerminator);
	message4.appendString(file4);
	message4.append(FileManagementService::VariableStringTerminator);

	MessageParser::execute(message4);
	CHECK(ServiceTests::countErrors() == 3);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// File name contains a wildcard
	Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<1024> repo5 = "test1";
	String<1024> file5 = "test2*";
	message5.appendString(repo5);
	message5.append(FileManagementService::VariableStringTerminator);
	message5.appendString(file5);
	message5.append(FileManagementService::VariableStringTerminator);

	MessageParser::execute(message5);
	CHECK(ServiceTests::countErrors() == 4);
	CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));

	// Object's path string size is too large
	Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<1024> repo6 = "test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1";
	String<1024> file6 = "test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2test2";
	message6.appendString(repo6);
	message6.append(FileManagementService::VariableStringTerminator);
	message6.appendString(file6);
	message6.append(FileManagementService::VariableStringTerminator);

	MessageParser::execute(message6);
	CHECK(ServiceTests::countErrors() == 5);
	CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfStringIsOutOfBounds));

	// The object's type is invalid
	Message message7(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<64> repo7 = "test1";
	String<64> file7 = "test5";
	message7.appendString(repo7);
	message7.append(FileManagementService::VariableStringTerminator);
	message7.appendString(file7);
	message7.append(FileManagementService::VariableStringTerminator);

	MessageParser::execute(message7);
	CHECK(ServiceTests::countErrors() == 6);
	CHECK(ServiceTests::thrownError(ErrorHandler::ObjectDoesNotExist));

	fs::create_directories("test1/test8");
	// The object's type a directory, not a file
	Message message8(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                 Message::TC, 0);
	String<64> repo8 = "test1";
	String<64> file8 = "test8";
	message8.appendString(repo8);
	message8.append(FileManagementService::VariableStringTerminator);
	message8.appendString(file8);
	message8.append(FileManagementService::VariableStringTerminator);

	MessageParser::execute(message8);
	CHECK(ServiceTests::countErrors() == 7);
	CHECK(ServiceTests::thrownError(ErrorHandler::AttemptedReportAttributesOnDirectory));

	fs::remove_all(fs::temp_directory_path() / "test1");
}

TEST_CASE("File attributes report TM[23,4]", "[service][st23]") {
	fs::current_path(fs::temp_directory_path());
	fs::create_directory("test1");
	std::ofstream file(fs::temp_directory_path() / "test1/test2");
	String<ECSSMaxStringSize> fileContents = "Hello, world!";
	file << fileContents.data();
	file.close();

	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                Message::TC, 0);
	String<64> repo1 = "test1";
	String<64> file1 = "test2";
	message.appendString(repo1);
	message.append(FileManagementService::VariableStringTerminator);
	message.appendString(file1);
	message.append(FileManagementService::VariableStringTerminator);

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
	CHECK(report.readByte() == FileManagementService::VariableStringTerminator);
	CHECK(report.readByte() == 't');
	CHECK(report.readByte() == 'e');
	CHECK(report.readByte() == 's');
	CHECK(report.readByte() == 't');
	CHECK(report.readByte() == '2');
	CHECK(report.readSint32() == fileContents.size());
	CHECK(report.readBoolean() == false);

	fs::remove_all(fs::temp_directory_path() / "test1");
}
