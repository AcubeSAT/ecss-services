#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/FileManagementService.hpp>
#include <catch2/catch_all.hpp>
#include <etl/String.hpp>
#include <filesystem>
#include <fstream>
#include "Helpers/FileCopyOperationIdGenerator.hpp"
#include "ServiceTests.hpp"

namespace fs = std::filesystem;

// @todo (#277): Add more tests covering cases where the repository and/or the object path is too long.
// For more information check https://gitlab.com/acubesat/obc/ecss-services/-/merge_requests/80#note_1610840164

TEST_CASE("Create a file TC[23,1]", "[service][st23]") {
	fs::create_directories("st23");

	SECTION("Good scenario") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> file1 = "create_file_1";
		message.appendOctetString(repo1);
		message.appendOctetString(file1);

		bool isFileLocked = false;
		message.appendBoolean(isFileLocked);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 0);
		CHECK(fs::exists("st23/create_file_1"));
	}

	SECTION("Max file size is too big") {
		Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo4 = "st23";
		String<ECSSTCRequestStringSize> file4 = "file2";
		message4.appendOctetString(repo4);
		message4.appendOctetString(file4);

		message4.appendUint32(FileManagementService::MaxPossibleFileSizeBytes + 10);
		bool isFileLocked = false;
		message4.appendBoolean(isFileLocked);

		MessageParser::execute(message4);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::SizeOfFileIsOutOfBounds));
	}

	SECTION("Repository name has a wildcard") {
		Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo5 = "test1*";
		String<ECSSTCRequestStringSize> file5 = "test2";
		message5.appendOctetString(repo5);
		message5.appendOctetString(file5);

		uint32_t maxFileSizeBytes = 100;
		message5.appendUint32(maxFileSizeBytes);
		bool isFileLocked = false;
		message5.appendBoolean(isFileLocked);

		MessageParser::execute(message5);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("The repository path leads to a file instead of a directory") {
		std::ofstream file("st23/create_file_2");
		file.close();
		Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo6 = "st23/create_file_2";
		String<ECSSTCRequestStringSize> file6 = "test3";
		message6.appendOctetString(repo6);
		message6.appendOctetString(file6);

		uint32_t maxFileSizeBytes = 100;
		message6.appendUint32(maxFileSizeBytes);
		bool isFileLocked = false;
		message6.appendBoolean(isFileLocked);

		MessageParser::execute(message6);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));
	}

	SECTION("File name has a wildcard") {
		Message message10(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
		                  0);
		String<1024> repo10 = "test1";
		String<1024> file10 = "test2*";
		message10.appendOctetString(repo10);
		message10.appendOctetString(file10);
		uint32_t maxFileSizeBytes = 100;
		message10.appendUint32(maxFileSizeBytes);

		MessageParser::execute(message10);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("File already exists") {
		fs::create_directories("st23/create_file_1");
		Message message11(FileManagementService::ServiceType, FileManagementService::MessageType::CreateFile, Message::TC,
		                  0);
		String<1024> repo11 = "st23";
		String<1024> file11 = "create_file_1";
		message11.appendOctetString(repo11);
		message11.appendOctetString(file11);
		uint32_t maxFileSizeBytes = 100;
		message11.appendUint32(maxFileSizeBytes);

		MessageParser::execute(message11);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::FileAlreadyExists));
	}

	fs::remove_all("st23");
}

TEST_CASE("Delete a file TC[23,2]", "[service][st23]") {
	fs::create_directories("st23");
	std::ofstream fileToRemove("st23/file_to_remove_1");
	fileToRemove.close();

	SECTION("Good scenario") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> file1 = "file_to_remove_1";
		message.appendOctetString(repo1);
		message.appendOctetString(file1);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 0);
		CHECK(fs::exists("st23/file_to_remove_1") == false);
	}

	SECTION("Repository name has a wildcard") {
		Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo4 = "test1*";
		String<ECSSTCRequestStringSize> file4 = "test2";
		message4.appendOctetString(repo4);
		message4.appendOctetString(file4);

		MessageParser::execute(message4);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("File name has a wildcard") {
		Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
		                 0);
		String<1024> repo5 = "test1";
		String<1024> file5 = "test2*";
		message5.appendOctetString(repo5);
		message5.appendOctetString(file5);

		MessageParser::execute(message5);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("Object's repository is a file, so it cannot be removed with this TC") {
		std::ofstream fileAsRepository("st23/file_to_remove_2");
		fileAsRepository.close();
		Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteFile, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo9 = "st23/file_to_remove_2";
		String<ECSSTCRequestStringSize> file9 = "test4";
		message9.appendOctetString(repo9);
		message9.appendOctetString(file9);

		MessageParser::execute(message9);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));
	}

	fs::remove_all("st23");
}

TEST_CASE("Report attributes of a file TC[23,3]", "[service][st23]") {
	fs::create_directory("st23");
	std::ofstream file("st23/file_attribute_1");
	String<ECSSMaxStringSize> fileContents = "Hello, world!";
	file << fileContents.data();
	file.close();
	CHECK(fs::exists("st23/file_attribute_1"));

	SECTION("Good scenario") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
		                Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> file1 = "file_attribute_1";
		message.appendOctetString(repo1);
		message.appendOctetString(file1);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 0);
	}

	SECTION("Repository name has a wildcard") {
		Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
		                 Message::TC, 0);
		String<ECSSTCRequestStringSize> repo4 = "test1*";
		String<ECSSTCRequestStringSize> file4 = "test2";
		message4.appendOctetString(repo4);
		message4.appendOctetString(file4);

		MessageParser::execute(message4);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("File name has a wildcard") {
		Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
		                 Message::TC, 0);
		String<1024> repo5 = "test1";
		String<1024> file5 = "test2*";
		message5.appendOctetString(repo5);
		message5.appendOctetString(file5);

		MessageParser::execute(message5);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("The object is not existent") {
		Message message7(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
		                 Message::TC, 0);
		String<ECSSTCRequestStringSize> repo7 = "st23";
		String<ECSSTCRequestStringSize> file7 = "missing_file_1";
		message7.appendOctetString(repo7);
		message7.appendOctetString(file7);

		MessageParser::execute(message7);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ObjectDoesNotExist));
	}

	SECTION("The object's type a directory, not a file") {
		fs::create_directories("st23/directory_1");
		Message message8(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
		                 Message::TC, 0);
		String<ECSSTCRequestStringSize> repo8 = "st23";
		String<ECSSTCRequestStringSize> file8 = "directory_1";
		message8.appendOctetString(repo8);
		message8.appendOctetString(file8);

		MessageParser::execute(message8);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::AttemptedReportAttributesOnDirectory));
	}

	fs::remove_all("st23");
}

TEST_CASE("File attributes report TM[23,4]", "[service][st23]") {
	fs::create_directory("st23");
	std::ofstream file("st23/report");
	String<ECSSMaxStringSize> fileContents = "Hello, world!";
	file << fileContents.data();
	file.close();

	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ReportAttributes,
	                Message::TC, 0);
	String<ECSSTCRequestStringSize> repo1 = "st23";
	String<ECSSTCRequestStringSize> file1 = "report";
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

	fs::remove_all("st23");
}

TEST_CASE("Lock a file TC[23,5]", "[service][st23]") {
	fs::create_directory("st23");

	SECTION("Good scenario") {
		std::ofstream fileToLock("st23/file_to_lock");
		fileToLock.close();
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::LockFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> file1 = "file_to_lock";
		message.appendOctetString(repo1);
		message.appendOctetString(file1);

		MessageParser::execute(message);
		fs::perms permissions = fs::status("st23/file_to_lock").permissions();
		CHECK(ServiceTests::countErrors() == 0);
		CHECK((permissions & fs::perms::owner_write) == fs::perms::none);
	}

	SECTION("Repository name has a wildcard") {
		Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::LockFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo4 = "test1*";
		String<ECSSTCRequestStringSize> file4 = "test2";
		message4.appendOctetString(repo4);
		message4.appendOctetString(file4);

		MessageParser::execute(message4);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("File name has a wildcard") {
		Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::LockFile, Message::TC, 0);
		String<1024> repo5 = "test1";
		String<1024> file5 = "test2*";
		message5.appendOctetString(repo5);
		message5.appendOctetString(file5);

		MessageParser::execute(message5);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("File does not exist") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::LockFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> file1 = "file_to_lock";
		message.appendOctetString(repo1);
		message.appendOctetString(file1);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ObjectDoesNotExist));
	}

	SECTION("Object's repository is a directory, so it cannot be locked with this TC") {
		fs::create_directories("st23/directory_1");
		Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::LockFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo9 = "st23";
		String<ECSSTCRequestStringSize> file9 = "directory_1";
		message9.appendOctetString(repo9);
		message9.appendOctetString(file9);

		MessageParser::execute(message9);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::AttemptedAccessModificationOnDirectory));
	}

	fs::remove_all("st23");
}

TEST_CASE("Unlock a file TC[23,6]", "[service][st23]") {
	fs::create_directory("st23");

	SECTION("Good scenario") {
		std::ofstream fileToLock("st23/file_to_unlock");
		fileToLock.close();
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::UnlockFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> file1 = "file_to_unlock";
		message.appendOctetString(repo1);
		message.appendOctetString(file1);

		MessageParser::execute(message);
		fs::perms permissions = fs::status("st23/file_to_unlock").permissions();
		CHECK(ServiceTests::countErrors() == 0);
		CHECK((permissions & fs::perms::owner_write) == fs::perms::owner_write);
	}

	SECTION("Repository name has a wildcard") {
		Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::UnlockFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo4 = "test1*";
		String<ECSSTCRequestStringSize> file4 = "test2";
		message4.appendOctetString(repo4);
		message4.appendOctetString(file4);

		MessageParser::execute(message4);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("File name has a wildcard") {
		Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::UnlockFile, Message::TC, 0);
		String<1024> repo5 = "test1";
		String<1024> file5 = "test2*";
		message5.appendOctetString(repo5);
		message5.appendOctetString(file5);

		MessageParser::execute(message5);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("File does not exist") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::UnlockFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> file1 = "file_to_lock";
		message.appendOctetString(repo1);
		message.appendOctetString(file1);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ObjectDoesNotExist));
	}

	SECTION("Object's repository is a directory, so it cannot be unlocked with this TC") {
		fs::create_directories("st23/directory_1");
		Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::UnlockFile, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo9 = "st23";
		String<ECSSTCRequestStringSize> file9 = "directory_1";
		message9.appendOctetString(repo9);
		message9.appendOctetString(file9);

		MessageParser::execute(message9);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::AttemptedAccessModificationOnDirectory));
	}

	fs::remove_all("st23");
}

TEST_CASE("Create a directory TC[23,9]", "[service][st23]") {
	fs::create_directories("st23");

	SECTION("Good scenario") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CreateDirectory, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> directory1 = "create_directory_1";
		message.appendOctetString(repo1);
		message.appendOctetString(directory1);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 0);
		CHECK(fs::exists("st23/create_directory_1"));
	}

	SECTION("Repository name has a wildcard") {
		Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::CreateDirectory, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo5 = "test1*";
		String<ECSSTCRequestStringSize> file5 = "test2";
		message5.appendOctetString(repo5);
		message5.appendOctetString(file5);

		MessageParser::execute(message5);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("The repository path leads to a file instead of a directory") {
		std::ofstream file("st23/create_file_2");
		file.close();
		Message message6(FileManagementService::ServiceType, FileManagementService::MessageType::CreateDirectory, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo6 = "st23/create_file_2";
		String<ECSSTCRequestStringSize> file6 = "test3";
		message6.appendOctetString(repo6);
		message6.appendOctetString(file6);

		MessageParser::execute(message6);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));
	}

	SECTION("Directory name has a wildcard") {
		Message message10(FileManagementService::ServiceType, FileManagementService::MessageType::CreateDirectory, Message::TC,
		                  0);
		String<1024> repo10 = "test1";
		String<1024> directory10 = "test2*";
		message10.appendOctetString(repo10);
		message10.appendOctetString(directory10);

		MessageParser::execute(message10);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("File already exists") {
		fs::create_directories("st23/create_directory_1");
		Message message11(FileManagementService::ServiceType, FileManagementService::MessageType::CreateDirectory, Message::TC,
		                  0);
		String<ECSSTCRequestStringSize> repo11 = "st23";
		String<ECSSTCRequestStringSize> directory11 = "create_directory_1";
		message11.appendOctetString(repo11);
		message11.appendOctetString(directory11);

		MessageParser::execute(message11);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::DirectoryAlreadyExists));
	}

	fs::remove_all("st23");
}

TEST_CASE("Delete a directory TC[23,10]", "[service][st23]") {
	fs::create_directories("st23/directory_to_remove_1");

	SECTION("Good scenario") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory, Message::TC, 0);
		String<ECSSTCRequestStringSize> repo1 = "st23";
		String<ECSSTCRequestStringSize> directory1 = "directory_to_remove_1";
		message.appendOctetString(repo1);
		message.appendOctetString(directory1);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 0);
		CHECK(fs::exists("st23/directory_to_remove_1") == false);
	}

	SECTION("Directory does not exist") {
		fs::remove("st23/directory_to_remove_1");
		Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo2 = "st23";
		String<ECSSTCRequestStringSize> directory2 = "directory_to_remove_1";
		message2.appendOctetString(repo2);
		message2.appendOctetString(directory2);

		MessageParser::execute(message2);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ObjectDoesNotExist));
	}

	SECTION("Repository name has a wildcard") {
		Message message4(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo4 = "test1*";
		String<ECSSTCRequestStringSize> file4 = "test2";
		message4.appendOctetString(repo4);
		message4.appendOctetString(file4);

		MessageParser::execute(message4);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("Directory name has a wildcard") {
		Message message5(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory, Message::TC,
		                 0);
		String<1024> repo5 = "test1";
		String<1024> directory5 = "test2*";
		message5.appendOctetString(repo5);
		message5.appendOctetString(directory5);

		MessageParser::execute(message5);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::UnexpectedWildcard));
	}

	SECTION("Object's repository is a file, so it cannot be removed with this TC") {
		std::ofstream fileAsRepository("st23/file");
		fileAsRepository.close();
		Message message9(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory, Message::TC,
		                 0);
		String<ECSSTCRequestStringSize> repo9 = "st23/file";
		String<ECSSTCRequestStringSize> directory9 = "test4";
		message9.appendOctetString(repo9);
		message9.appendOctetString(directory9);

		MessageParser::execute(message9);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::RepositoryPathLeadsToFile));
	}

	SECTION("Object's repository is a file, so it cannot be removed with this TC") {
		fs::create_directories("st23/directory_to_remove_2");
		std::ofstream fileAsRepository2("st23/directory_to_remove_2/file");
		fileAsRepository2.close();
		Message message10(FileManagementService::ServiceType, FileManagementService::MessageType::DeleteDirectory, Message::TC,
		                  0);
		String<ECSSTCRequestStringSize> repo10 = "st23";
		String<ECSSTCRequestStringSize> directory10 = "directory_to_remove_2";
		message10.appendOctetString(repo10);
		message10.appendOctetString(directory10);

		MessageParser::execute(message10);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::AttemptedDeleteNonEmptyDirectory));
	}

	fs::remove_all("st23");
}

TEST_CASE("Copy a file TC[23,14]", "[service][st23]") {
	fs::create_directories("st23/source");
	fs::create_directories("st23/destination");
	std::ofstream file("st23/source/file");
	file.close();
	std::ofstream file1("st23/destination/file1");
	file1.close();
	fs::create_directories("remote/source");
	fs::create_directories("remote/destination");
	std::ofstream file2("remote/source/file");
	file2.close();

	SECTION("Good scenario") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
		message.appendUint32(Filesystem::OperationIdGenerator::next());
		String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
		String<ECSSTCRequestStringSize> sourceFile = "file";
		message.appendOctetString(sourceRepo);
		message.appendOctetString(sourceFile);
		String<ECSSTCRequestStringSize> destinationRepo = "st23/destination";
		message.appendOctetString(destinationRepo);
		message.appendOctetString(sourceFile);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 0);
		CHECK(fs::exists("st23/source/file"));
		CHECK(fs::exists("st23/destination/file"));
	}

	SECTION("Copy operation ID is in use") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
		Filesystem::OperationIdGenerator::markInUse(1);
		message.appendUint32(1);
		String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
		String<ECSSTCRequestStringSize> sourceFile = "file";
		message.appendOctetString(sourceRepo);
		message.appendOctetString(sourceFile);
		String<ECSSTCRequestStringSize> destinationRepo = "st23/destination";
		message.appendOctetString(destinationRepo);
		message.appendOctetString(sourceFile);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdAlreadyInUse));
	}

	SECTION("Invalid source directory path") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
		message.appendUint32(Filesystem::OperationIdGenerator::next());
		String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
		String<ECSSTCRequestStringSize> sourceFile = "file---";
		message.appendOctetString(sourceRepo);
		message.appendOctetString(sourceFile);
		String<ECSSTCRequestStringSize> destinationRepo = "st23/destination";
		message.appendOctetString(destinationRepo);
		message.appendOctetString(sourceFile);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid));
	}

	SECTION("Copy operation requested from remote filesystem to remote filesystem") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
		message.appendUint32(Filesystem::OperationIdGenerator::next());
		String<ECSSTCRequestStringSize> sourceRepo = "remote/source";
		String<ECSSTCRequestStringSize> sourceFile = "file";
		message.appendOctetString(sourceRepo);
		message.appendOctetString(sourceFile);
		String<ECSSTCRequestStringSize> destinationRepo = "remote/destination";
		message.appendOctetString(destinationRepo);
		message.appendOctetString(sourceFile);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::FileCopyRequestedFromRemoteToRemoteRepository));
	}

	SECTION("Source path leads to directory") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
		message.appendUint32(Filesystem::OperationIdGenerator::next());
		String<ECSSTCRequestStringSize> sourceRepo = "st23";
		String<ECSSTCRequestStringSize> sourceFile = "source";
		message.appendOctetString(sourceRepo);
		message.appendOctetString(sourceFile);
		String<ECSSTCRequestStringSize> destinationRepo = "st23/destination";
		message.appendOctetString(destinationRepo);
		message.appendOctetString(sourceFile);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionCompletionErrorType::AttemptedCopyFileOperationOnDirectory));
	}

	SECTION("Destination file already exists") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
		message.appendUint32(Filesystem::OperationIdGenerator::next());

		String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
		String<ECSSTCRequestStringSize> sourceFile = "file";
		message.appendOctetString(sourceRepo);
		message.appendOctetString(sourceFile);
		String<ECSSTCRequestStringSize> destinationRepo = "st23/destination";
		String<ECSSTCRequestStringSize> destinationFile = "file1";
		message.appendOctetString(destinationRepo);
		message.appendOctetString(destinationFile);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionCompletionErrorType::DestinationFileAlreadyExists));
	}

	SECTION("Insufficient memory") {
		Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
		message.appendUint32(Filesystem::OperationIdGenerator::next());
		const std::string longText = "This text is definitely longer than forty-two bytes.\n";
		if (std::ofstream outFile("st23/source/test_file.txt"); outFile.is_open()) {
			outFile << longText;
			outFile.close();
		}
		String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
		String<ECSSTCRequestStringSize> sourceFile = "test_file.txt";
		message.appendOctetString(sourceRepo);
		message.appendOctetString(sourceFile);
		String<ECSSTCRequestStringSize> destinationRepo = "st23/destination";
		String<ECSSTCRequestStringSize> destinationFile = "file2.txt";
		message.appendOctetString(destinationRepo);
		message.appendOctetString(destinationFile);

		MessageParser::execute(message);
		CHECK(ServiceTests::countErrors() == 1);
		CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionCompletionErrorType::FileSystemInsufficientSpace));
	}
}

TEST_CASE("Observe available unallocated memory 6.23.4.7", "[service][st23]") {
	uint32_t freeMemory = Services.fileManagement.getUnallocatedMemory();
	CHECK(freeMemory == 42);
}