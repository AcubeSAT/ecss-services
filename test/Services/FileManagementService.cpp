#include <Message.hpp>
#include <ServicePool.hpp>
#include <Services/FileManagementService.hpp>
#include <catch2/catch_all.hpp>
#include <chrono>
#include <etl/String.hpp>
#include <filesystem>
#include <fstream>
#include <thread>
#include <unistd.h>

#include "ServiceTests.hpp"

namespace fs = std::filesystem;

// @todo (#277): Add more tests covering cases where the repository and/or the object path is too long.
// For more information check https://gitlab.com/acubesat/obc/ecss-services/-/merge_requests/80#note_1610840164

void waitForOperationCompletion(const uint16_t operationId, const int timeoutMs = 2000) {
    const auto start = std::chrono::steady_clock::now();
    while (true) {
       if (const auto* op = Services.fileManagement.findFileCopyOperation(operationId);
          !op || op->getState() == FileManagementService::FileCopyOperation::State::COMPLETED ||
           op->getState() == FileManagementService::FileCopyOperation::State::FAILED) {
          break;
       }
       if (const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count(); elapsed > timeoutMs) {
          break;
       }
       std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

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

TEST_CASE("Copy a file TC[23,14], service st23", "[service][st23]") {
    Filesystem::initializeFileSystems();
    Filesystem::initializeFileCopyTask();

    auto createFile = [](const char* path, const char* data = "") {
        std::ofstream f(path);
        f << data;
    };

    SECTION("Good scenario - successful execution notification") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::remove_all("remote");
        fs::create_directories("st23/source");
        fs::create_directories("st23/destination");
        createFile("st23/source/file", "data");
        fs::remove("st23/destination/file");
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        message.appendUint16(1);
        String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
        String<ECSSTCRequestStringSize> sourceFile = "file";
        String<ECSSTCRequestStringSize> destRepo   = "st23/destination";
        message.appendOctetString(sourceRepo);
        message.appendOctetString(sourceFile);
        message.appendOctetString(destRepo);
        message.appendOctetString(sourceFile);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        waitForOperationCompletion(1);
        CHECK(fs::exists("st23/source/file"));
        CHECK(fs::exists("st23/destination/file"));
    }

    SECTION("Copy operation ID is in use") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::remove_all("remote");
        fs::create_directories("st23/source");
        fs::create_directories("st23/destination");
        createFile("st23/source/file", "data");
        Message message1(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        message1.appendUint16(100);
        String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
        String<ECSSTCRequestStringSize> sourceFile = "file";
        String<ECSSTCRequestStringSize> destRepo   = "st23/destination";
        message1.appendOctetString(sourceRepo);
        message1.appendOctetString(sourceFile);
        message1.appendOctetString(destRepo);
        message1.appendOctetString(sourceFile);
        MessageParser::execute(message1);
        CHECK(ServiceTests::countErrors() == 0);
        Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        message2.appendUint16(100);
        message2.appendOctetString(sourceRepo);
        message2.appendOctetString(sourceFile);
        message2.appendOctetString(destRepo);
        message2.appendOctetString(sourceFile);
        MessageParser::execute(message2);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdAlreadyInUse));
        waitForOperationCompletion(100);
    }

    SECTION("Invalid source directory path - fails at START verification") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::remove_all("remote");
        fs::create_directories("st23/source");
        fs::create_directories("st23/destination");
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        message.appendUint16(2);
        String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
        String<ECSSTCRequestStringSize> sourceFile = "file---";
        String<ECSSTCRequestStringSize> destRepo   = "st23/destination";
        message.appendOctetString(sourceRepo);
        message.appendOctetString(sourceFile);
        message.appendOctetString(destRepo);
        message.appendOctetString(sourceFile);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::ObjectPathIsInvalid));
    }

    SECTION("Remote to remote - fails at START verification") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::remove_all("remote");
        fs::create_directories("remote/source");
        fs::create_directories("remote/destination");
        createFile("remote/source/file", "data");
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        message.appendUint16(3);
        String<ECSSTCRequestStringSize> sourceRepo = "remote/source";
        String<ECSSTCRequestStringSize> sourceFile = "file";
        String<ECSSTCRequestStringSize> destRepo   = "remote/destination";
        message.appendOctetString(sourceRepo);
        message.appendOctetString(sourceFile);
        message.appendOctetString(destRepo);
        message.appendOctetString(sourceFile);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::FileCopyOperationRequestedFromRemoteToRemoteRepository));
    }

    SECTION("Locked file - fails at START verification") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::remove_all("remote");
        fs::create_directories("st23/source");
        fs::create_directories("st23/destination");
        createFile("st23/source/lockedFile", "data");
        Message lockFileMessage(FileManagementService::ServiceType, FileManagementService::MessageType::LockFile, Message::TC, 0);
        String<ECSSTCRequestStringSize> repo       = "st23/source";
        String<ECSSTCRequestStringSize> lockedFile = "lockedFile";
        lockFileMessage.appendOctetString(repo);
        lockFileMessage.appendOctetString(lockedFile);
        MessageParser::execute(lockFileMessage);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        message.appendUint16(4);
        String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
        String<ECSSTCRequestStringSize> sourceFile = "lockedFile";
        String<ECSSTCRequestStringSize> destRepo   = "st23/destination";
        message.appendOctetString(sourceRepo);
        message.appendOctetString(sourceFile);
        message.appendOctetString(destRepo);
        message.appendOctetString(sourceFile);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::FileCopyOperationRequestedOnLockedFile));
    }

    SECTION("Destination file already exists - fails at COMPLETION verification") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::remove_all("remote");
        fs::create_directories("st23/source");
        fs::create_directories("st23/destination");
        createFile("st23/source/file", "src-data");
        createFile("st23/destination/file", "dest-data");
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        message.appendUint16(10);
        String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
        String<ECSSTCRequestStringSize> sourceFile = "file";
        String<ECSSTCRequestStringSize> destRepo   = "st23/destination";
        message.appendOctetString(sourceRepo);
        message.appendOctetString(sourceFile);
        message.appendOctetString(destRepo);
        message.appendOctetString(sourceFile);
        MessageParser::execute(message);
        waitForOperationCompletion(10);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionCompletionErrorType::DestinationFileAlreadyExists));
        CHECK(fs::exists("st23/source/file"));
        CHECK(fs::exists("st23/destination/file"));
    }

    Filesystem::shutdownFileCopyTask();
    fs::remove_all("st23");
    fs::remove_all("remote");
}

TEST_CASE("Move a file TC[23,15], service st23", "[service][st23]") {
    Filesystem::initializeFileSystems();
    Filesystem::initializeFileCopyTask();

    auto createFile = [](const char* path, const char* data = "") {
        std::ofstream f(path);
        f << data;
    };

    SECTION("Good scenario - successful execution notification") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::create_directories("st23/source");
        fs::create_directories("st23/destination");
        createFile("st23/source/file", "data");
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::MoveFile, Message::TC, 0);
        message.appendUint16(1);
        String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
        String<ECSSTCRequestStringSize> sourceFile = "file";
        String<ECSSTCRequestStringSize> destRepo = "st23/destination";
        message.appendOctetString(sourceRepo);
        message.appendOctetString(sourceFile);
        message.appendOctetString(destRepo);
        message.appendOctetString(sourceFile);
        MessageParser::execute(message);
		waitForOperationCompletion(1);
		CHECK(ServiceTests::countErrors() == 0);
		CHECK(fs::exists("st23/destination/file"));
		CHECK(!fs::exists("st23/source/file"));
	}

    SECTION("Move operation ID is in use") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::create_directories("st23/source");
        fs::create_directories("st23/destination");
        createFile("st23/source/file", "data");
        fs::remove("st23/destination/file");
        Message message1(FileManagementService::ServiceType, FileManagementService::MessageType::MoveFile, Message::TC, 0);
        message1.appendUint16(100);
        String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
        String<ECSSTCRequestStringSize> sourceFile = "file";
        String<ECSSTCRequestStringSize> destRepo = "st23/destination";
        message1.appendOctetString(sourceRepo);
        message1.appendOctetString(sourceFile);
        message1.appendOctetString(destRepo);
        message1.appendOctetString(sourceFile);
        MessageParser::execute(message1);
        Message message2(FileManagementService::ServiceType, FileManagementService::MessageType::MoveFile, Message::TC, 0);
        message2.appendUint16(100);
        message2.appendOctetString(sourceRepo);
        message2.appendOctetString(sourceFile);
        message2.appendOctetString(destRepo);
        message2.appendOctetString(sourceFile);
        MessageParser::execute(message2);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdAlreadyInUse));
        waitForOperationCompletion(100);
    }

    SECTION("Destination file already exists - fails at COMPLETION verification") {
        ServiceTests::reset();
        fs::remove_all("st23");
        fs::create_directories("st23/source");
        fs::create_directories("st23/destination");
        createFile("st23/source/file", "src-data");
        createFile("st23/destination/file", "src-data");
        constexpr uint16_t operationId = 11;
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::MoveFile, Message::TC, 0);
        message.appendUint16(operationId);
        String<ECSSTCRequestStringSize> sourceRepo = "st23/source";
        String<ECSSTCRequestStringSize> sourceFile = "file";
        String<ECSSTCRequestStringSize> destRepo   = "st23/destination";
        message.appendOctetString(sourceRepo);
        message.appendOctetString(sourceFile);
        message.appendOctetString(destRepo);
        message.appendOctetString(sourceFile);
        MessageParser::execute(message);
        waitForOperationCompletion(operationId);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionCompletionErrorType::DestinationFileAlreadyExists));
        CHECK(fs::exists("st23/source/file"));
        CHECK(fs::exists("st23/destination/file"));
    }

    Filesystem::shutdownFileCopyTask();
    fs::remove_all("st23");
}

TEST_CASE("Suspend file copy operations TC[23,16]", "[service][st23]") {
    SECTION("Suspend single operation by ID") {
        ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        Services.fileManagement.addFileCopyOperation(100, "source/file", "dest/file",
        	FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        auto* op = Services.fileManagement.findFileCopyOperation(100);
        REQUIRE(op != nullptr);
        op->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::SuspendFileCopyOperation, Message::TC, 0);
        message.appendUint8(1);
        message.appendUint16(100);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        CHECK(op->getState() == FileManagementService::FileCopyOperation::State::ON_HOLD);
    }

    SECTION("Suspend multiple operations") {
        ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType,FileManagementService::MessageType::CopyFile, Message::TC, 0);
        Services.fileManagement.addFileCopyOperation(101, "source/file1", "dest/file1",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        Services.fileManagement.addFileCopyOperation(102, "source/file2", "dest/file2",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        auto* op1 = Services.fileManagement.findFileCopyOperation(101);
        auto* op2 = Services.fileManagement.findFileCopyOperation(102);
        op1->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        op2->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::SuspendFileCopyOperation, Message::TC, 0);
        message.appendUint8(2);
        message.appendUint16(101);
        message.appendUint16(102);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        CHECK(op1->getState() == FileManagementService::FileCopyOperation::State::ON_HOLD);
        CHECK(op2->getState() == FileManagementService::FileCopyOperation::State::ON_HOLD);
    }

    SECTION("Suspend non-existent operation") {
        ServiceTests::reset();
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::SuspendFileCopyOperation, Message::TC, 0);
        message.appendUint8(1);
        message.appendUint16(999);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::FileCopyOperationIdNotFound));
    }
}

TEST_CASE("Resume file copy operations TC[23,17]", "[service][st23]") {
    SECTION("Resume single operation") {
        ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        Services.fileManagement.addFileCopyOperation(200, "source/file", "dest/file",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        auto* op = Services.fileManagement.findFileCopyOperation(200);
        op->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        CHECK(op->getState() == FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        op->setState(FileManagementService::FileCopyOperation::State::ON_HOLD);
        CHECK(op->getState() == FileManagementService::FileCopyOperation::State::ON_HOLD);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ResumeFileCopyOperation, Message::TC, 0);
        message.appendUint8(1);
        message.appendUint16(200);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        CHECK(op->getState() == FileManagementService::FileCopyOperation::State::IN_PROGRESS);
    }

    SECTION("Resume multiple operations") {
        ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        Services.fileManagement.addFileCopyOperation(201, "source/file1", "dest/file1",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        Services.fileManagement.addFileCopyOperation(202, "source/file2", "dest/file2",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        auto* op1 = Services.fileManagement.findFileCopyOperation(201);
        auto* op2 = Services.fileManagement.findFileCopyOperation(202);
        op1->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        op1->setState(FileManagementService::FileCopyOperation::State::ON_HOLD);
        op2->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        op2->setState(FileManagementService::FileCopyOperation::State::ON_HOLD);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ResumeFileCopyOperation, Message::TC, 0);
        message.appendUint8(2);
        message.appendUint16(201);
        message.appendUint16(202);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        CHECK(op1->getState() == FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        CHECK(op2->getState() == FileManagementService::FileCopyOperation::State::IN_PROGRESS);
    }
}


TEST_CASE("Abort file copy operations TC[23,18]", "[service][st23]") {
    SECTION("Abort single operation") {
        ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        Services.fileManagement.addFileCopyOperation(300, "source/file", "dest/file",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        auto* op = Services.fileManagement.findFileCopyOperation(300);
        op->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::AbortFileCopyOperation, Message::TC, 0);
        message.appendUint8(1);
        message.appendUint16(300);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        CHECK(Services.fileManagement.findFileCopyOperation(300) == nullptr);
    }

    SECTION("Abort multiple operations") {
        ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        Services.fileManagement.addFileCopyOperation(301, "source/file1", "dest/file1",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        Services.fileManagement.addFileCopyOperation(302, "source/file2", "dest/file2",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        Services.fileManagement.findFileCopyOperation(301)->setState(
            FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        Services.fileManagement.findFileCopyOperation(302)->setState(
            FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::AbortFileCopyOperation, Message::TC, 0);
        message.appendUint8(2);
        message.appendUint16(301);
        message.appendUint16(302);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        CHECK(Services.fileManagement.findFileCopyOperation(301) == nullptr);
        CHECK(Services.fileManagement.findFileCopyOperation(302) == nullptr);
    }
}

TEST_CASE("Suspend/Resume/Abort operations in path TC[23,19-21]", "[service][st23]") {
    SECTION("Suspend operations in path") {
        ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        Services.fileManagement.addFileCopyOperation(400, "st23/source/file1", "st23/dest/file1",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        Services.fileManagement.addFileCopyOperation(401, "st23/source/file2", "st23/dest/file2",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        Services.fileManagement.addFileCopyOperation(402, "other/source/file", "other/dest/file",
                                                      FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        auto* op1 = Services.fileManagement.findFileCopyOperation(400);
        auto* op2 = Services.fileManagement.findFileCopyOperation(401);
        auto* op3 = Services.fileManagement.findFileCopyOperation(402);
        op1->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        op2->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        op3->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::SuspendFileCopyOperationInPath, Message::TC, 0);
        String<ECSSTCRequestStringSize> repo = "st23/source";
        message.appendOctetString(repo);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        CHECK(op1->getState() == FileManagementService::FileCopyOperation::State::ON_HOLD);
        CHECK(op2->getState() == FileManagementService::FileCopyOperation::State::ON_HOLD);
        CHECK(op3->getState() == FileManagementService::FileCopyOperation::State::IN_PROGRESS);
    }

	SECTION("Resume operations in path") {
    	ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
    	Services.fileManagement.addFileCopyOperation(403, "st23/source/file1", "st23/dest/file1",
													  FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);

    	auto* op = Services.fileManagement.findFileCopyOperation(403);
    	op->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
    	op->setState(FileManagementService::FileCopyOperation::State::ON_HOLD);
    	Message message(FileManagementService::ServiceType, FileManagementService::MessageType::ResumeFileCopyOperationInPath, Message::TC, 0);
    	String<ECSSTCRequestStringSize> repo = "st23/source";
    	message.appendOctetString(repo);
    	MessageParser::execute(message);
    	CHECK(ServiceTests::countErrors() == 0);
    	CHECK(op->getState() == FileManagementService::FileCopyOperation::State::IN_PROGRESS);
    }

    SECTION("Abort operations in path - success") {
        ServiceTests::reset();
    	Message dummyMessage(FileManagementService::ServiceType, FileManagementService::MessageType::CopyFile, Message::TC, 0);
        Services.fileManagement.addFileCopyOperation(404, "st23/source/file1",
        	"st23/dest/file1", FileManagementService::FileCopyOperation::Type::COPY, dummyMessage);
        Services.fileManagement.findFileCopyOperation(404)->setState(FileManagementService::FileCopyOperation::State::IN_PROGRESS);
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::AbortFileCopyOperationInPath, Message::TC, 0);
        String<ECSSTCRequestStringSize> repo = "st23/source";
        message.appendOctetString(repo);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
        CHECK(Services.fileManagement.findFileCopyOperation(404) == nullptr);
    }

    SECTION("Abort operations in path - no active operations") {
        ServiceTests::reset();
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::AbortFileCopyOperationInPath, Message::TC, 0);
        String<ECSSTCRequestStringSize> repo = "empty/path";
        message.appendOctetString(repo);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::NoActiveFileCopyOperationsFound));
    }
}

TEST_CASE("Periodic file copy status reporting TC[23,22-24]", "[service][st23]") {
    SECTION("Enable periodic reporting") {
        ServiceTests::reset();
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::EnablePeriodicReportingOfFileCopy, Message::TC, 0);
        message.appendUint16(1000);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
    }

    SECTION("Interval too small") {
        ServiceTests::reset();
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::EnablePeriodicReportingOfFileCopy, Message::TC, 0);
        message.appendUint16(50);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 1);
        CHECK(ServiceTests::thrownError(ErrorHandler::ExecutionStartErrorType::InvalidReportingInterval));
    }

    SECTION("Disable periodic reporting") {
        ServiceTests::reset();
        Message message(FileManagementService::ServiceType, FileManagementService::MessageType::DisablePeriodicReportingOfFileCopy, Message::TC, 0);
        MessageParser::execute(message);
        CHECK(ServiceTests::countErrors() == 0);
    }
}

TEST_CASE("Observe available unallocated memory 6.23.4.7", "[service][st23]") {
    uint32_t freeMemory = Services.fileManagement.getUnallocatedMemory();
    CHECK(freeMemory == 42);
}
