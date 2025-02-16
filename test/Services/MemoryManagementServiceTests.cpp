#include <fstream>
#include <iostream>
#include <Message.hpp>
#include <bits/fs_fwd.h>
#include <bits/fs_path.h>
#include <Services/MemoryManagementService.hpp>
#include <catch2/catch_all.hpp>
#include "Helpers/CRCHelper.hpp"
#include "ServiceTests.hpp"
#include <sys/stat.h>

MemoryManagementService& memMangService = Services.memoryManagement;
namespace fs = std::filesystem;

TEST_CASE("TC[6,2]", "[service][st06]") {
	// Required test variables
	char* pStr = static_cast<char*>(malloc(4));
	*pStr = 'T';
	*(pStr + 1) = 'G';
	*(pStr + 2) = '\0';
	uint8_t data[2] = {'h', 'R'};

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::LoadRawMemoryDataAreas, Message::TC, 1);
	receivedPacket.append<MemoryId>(MemoryManagementService::MemoryID::EXTERNAL); // Memory ID
	receivedPacket.appendUint16(2);                                          // Iteration count
	receivedPacket.append<StartAddress>(reinterpret_cast<StartAddress>(pStr));           // Start address
	receivedPacket.appendOctetString(String<2>(data));
	receivedPacket.appendBits(16, CRCHelper::calculateCRC(data, 2));   // Append CRC
	receivedPacket.append<StartAddress>(reinterpret_cast<StartAddress>(pStr + 2)); // Start address
	receivedPacket.appendOctetString(String<1>(data));                 // Append CRC
	receivedPacket.appendBits(16, CRCHelper::calculateCRC(data, 1));
	MessageParser::execute(receivedPacket);

	CHECK(pStr[0] == 'h');
	CHECK(pStr[1] == 'R');
	CHECK(pStr[2] == 'h');

	free(pStr);
}

TEST_CASE("TC[6,5]", "[service][st06]") {
	uint8_t testString_1[6] = "FStrT";
	uint8_t testString_2[8] = "SecStrT";
	uint8_t testString_3[2] = {5, 8};

	uint8_t checkString[ECSSMaxStringSize];
	uint16_t readSize = 0, checksum = 0;

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::DumpRawMemoryData, Message::TC, 1);
	receivedPacket.append<MemoryId>(MemoryManagementService::MemoryID::EXTERNAL);     // Memory ID
	receivedPacket.appendUint16(3);                                              // Iteration count (Equal to 3 test strings)
	receivedPacket.append<StartAddress>(reinterpret_cast<StartAddress>(testString_1));       // Start address
	receivedPacket.append<MemoryDataLength>(sizeof(testString_1) / sizeof(testString_1[0])); // Data read length

	receivedPacket.append<StartAddress>(reinterpret_cast<StartAddress>(testString_2));
	receivedPacket.append<MemoryDataLength>(sizeof(testString_2) / sizeof(testString_2[0]));

	receivedPacket.append<StartAddress>(reinterpret_cast<StartAddress>(testString_3));
	receivedPacket.append<MemoryDataLength>(sizeof(testString_3) / sizeof(testString_3[0]));
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == MemoryManagementService::ServiceType);
	CHECK(response.messageType == MemoryManagementService::MessageType::DumpRawMemoryDataReport);
	REQUIRE(response.dataSize == 55);

	CHECK(response.read<MemoryId>() == MemoryManagementService::MemoryID::EXTERNAL);
	CHECK(response.readUint16() == 3);
	CHECK(response.read<StartAddress>() == reinterpret_cast<StartAddress>(testString_1));
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(8*sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_1) / sizeof(testString_1[0]));
	CHECK(checkString[0] == 'F');
	CHECK(checkString[1] == 'S');
	CHECK(checkString[2] == 't');
	CHECK(checkString[3] == 'r');
	CHECK(checkString[4] == 'T');
	CHECK(checkString[5] == '\0');
	CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));

	CHECK(response.read<StartAddress>() == reinterpret_cast<StartAddress>(testString_2));
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(8*sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_2) / sizeof(testString_2[0]));
	CHECK(checkString[0] == 'S');
	CHECK(checkString[1] == 'e');
	CHECK(checkString[2] == 'c');
	CHECK(checkString[3] == 'S');
	CHECK(checkString[4] == 't');
	CHECK(checkString[5] == 'r');
	CHECK(checkString[6] == 'T');
	CHECK(checkString[7] == '\0');
	CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));

	CHECK(response.read<StartAddress>() == reinterpret_cast<StartAddress>(testString_3));
	readSize = response.readOctetString(checkString);
	checksum = response.readBits(8*sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_3) / sizeof(testString_3[0]));
	CHECK(checkString[0] == 5);
	CHECK(checkString[1] == 8);
	CHECK(checksum == CRCHelper::calculateCRC(checkString, readSize));
}

TEST_CASE("TC[6,9]", "[service][st06]") {
	uint8_t testString_1[6] = "FStrT";
	uint8_t testString_2[8] = "SecStrT";
	uint16_t readSize = 0, checksum = 0;

	Message receivedPacket = Message(MemoryManagementService::ServiceType, MemoryManagementService::MessageType::CheckRawMemoryData, Message::TC, 1);
	receivedPacket.append<MemoryId>(MemoryManagementService::MemoryID::EXTERNAL);     // Memory ID
	receivedPacket.appendUint16(2);                                              // Iteration count
	receivedPacket.append<StartAddress>(reinterpret_cast<StartAddress>(testString_1));       // Start address
	receivedPacket.append<MemoryDataLength>(sizeof(testString_1) / sizeof(testString_1[0])); // Data read length

	receivedPacket.append<StartAddress>(reinterpret_cast<StartAddress>(testString_2));
	receivedPacket.append<MemoryDataLength>(sizeof(testString_2) / sizeof(testString_2[0]));
	MessageParser::execute(receivedPacket);
	REQUIRE(ServiceTests::hasOneMessage());

	Message response = ServiceTests::get(0);
	CHECK(response.serviceType == MemoryManagementService::ServiceType);
	CHECK(response.messageType == MemoryManagementService::MessageType::CheckRawMemoryDataReport);
	REQUIRE(response.dataSize == 27);

	CHECK(response.read<MemoryId>() == MemoryManagementService::MemoryID::EXTERNAL);
	CHECK(response.readUint16() == 2);
	CHECK(response.read<StartAddress>() == reinterpret_cast<StartAddress>(testString_1));
	readSize = response.readUint16();
	checksum = response.readBits(8*sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_1) / sizeof(testString_1[0]));
	CHECK(checksum == CRCHelper::calculateCRC(testString_1, readSize));

	CHECK(response.read<StartAddress>() == reinterpret_cast<StartAddress>(testString_2));
	readSize = response.readUint16();
	checksum = response.readBits(8*sizeof(MemoryManagementChecksum));
	CHECK(readSize == sizeof(testString_2) / sizeof(testString_2[0]));
	CHECK(checksum == CRCHelper::calculateCRC(testString_2, readSize));
}


TEST_CASE("TC[6,1] Load Object Memory Data", "[service][st06]") {
	SECTION("Successful single instruction load") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::LoadObjectMemoryData, Message::TC, 1);
		
		String<FullPathSize> path = "memoryManagementTestFile.txt";
		request.appendOctetString(path);
		
		request.append<InstructionType>(1);
		
		const FileOffset offset = 0;
		const String<ChunkMaxFileSizeBytes> testData = "Hello World";
		const FileDataLength dataLength = testData.size();
		
		request.append<FileOffset>(offset);
		request.append<FileDataLength>(dataLength);
		request.appendString(testData);

		const char* filename = "memoryManagementTestFile.txt";
		std::filesystem::path fileNamePath = fs::path(filename);
		std::ofstream file(filename);
		for (size_t i = 0; i < 50; i++) {
			file << i << " block\n";
		}
		file.close();

		MessageParser::execute(request);
		Message response = ServiceTests::get(0);
		REQUIRE(ServiceTests::count() == 1);
		CHECK(response.messageType == RequestVerificationService::SuccessfulCompletionOfExecution);
		ServiceTests::reset();
	}
	SECTION("Multiple instruction load") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::LoadObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "memoryManagementTestFile.txt";
		request.appendOctetString(path);

		// Two instructions
		request.append<InstructionType>(2);

		// First data block
		const FileOffset offset1 = 0;
		const String<ChunkMaxFileSizeBytes> testData1 = "First block";
		const FileDataLength dataLength1 = testData1.size();

		request.append<FileOffset>(offset1);
		request.append<FileDataLength>(dataLength1);
		request.appendString(testData1);

		// Second data block
		const FileOffset offset2 = 100;
		const String<ChunkMaxFileSizeBytes> testData2 = "Second block";
		const FileDataLength dataLength2 = testData2.size();

		request.append<FileOffset>(offset2);
		request.append<FileDataLength>(dataLength2);
		request.appendString(testData2);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::hasOneMessage());
		Message response = ServiceTests::get(0);
		CHECK(response.messageType == RequestVerificationService::SuccessfulCompletionOfExecution);
		ServiceTests::reset();
	}

	SECTION("File not found error") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::LoadObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "/nonexistent/file.txt";
		request.appendOctetString(path);

		request.append<InstructionType>(1);

		const FileOffset offset = 0;
		const String<ChunkMaxFileSizeBytes> testData1 = "First block";
		const FileDataLength dataLength1 = testData1.size();

		request.append<FileOffset>(offset);
		request.append<FileDataLength>(dataLength1);
		request.appendString(testData1);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 2);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MemoryObjectDoesNotExist) == 1);

		Message completion = ServiceTests::get(1);
		CHECK(completion.messageType == RequestVerificationService::FailedCompletionOfExecution);
		ServiceTests::reset();
	}

	SECTION("Invalid offset error") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::LoadObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "memoryManagementTestFile.txt";
		request.appendOctetString(path);

		request.append<InstructionType>(1);

		const FileOffset offset = 0xFFFF;
		const String<ChunkMaxFileSizeBytes> testData = "Hello World";
		const FileDataLength dataLength = testData.size();

		request.append<FileOffset>(offset);
		request.append<FileDataLength>(dataLength);
		request.appendString(testData);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 2);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::InvalidMemoryOffset) == 1);

		Message completion = ServiceTests::get(1);
		CHECK(completion.messageType == RequestVerificationService::FailedCompletionOfExecution);
		ServiceTests::reset();
	}

	SECTION("Write error") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::LoadObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "memoryManagementTestBroken.txt";
		request.appendOctetString(path);

		request.append<InstructionType>(1);

		const FileOffset offset = 0;
		const String<ChunkMaxFileSizeBytes> testData = "Test data";
		const FileDataLength dataLength = testData.size();

		request.append<FileOffset>(offset);
		request.append<FileDataLength>(dataLength);
		request.appendString(testData);
		const char* filename = "memoryManagementTestBroken.txt";
		std::filesystem::path fileNamePath = fs::path(filename);
		std::ofstream file(filename);
		for (size_t i = 0; i < 50; i++) {
			file << i << " block\n";
		}
		file.close();

		#ifdef WIN32
		// Windows-specific code to make file read-only
		std::filesystem::permissions(fileNamePath, 
			std::filesystem::perms::owner_read |
			std::filesystem::perms::group_read |
			std::filesystem::perms::others_read);
		#else
		// Unix-specific code to make file read-only 
		chmod(filename, S_IRUSR | S_IRGRP | S_IROTH);
		#endif

		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MemoryWriteError) == 1);

		Message completion = ServiceTests::get(1);
		CHECK(completion.messageType == RequestVerificationService::FailedCompletionOfExecution);
		ServiceTests::reset();
	}
}

TEST_CASE("TC[6,3] Dump Object Memory Data", "[service][st06]") {
	SECTION("Successful single block dump") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "memoryManagementTestFile.txt";
		request.appendOctetString(path);

		const char* filename = "memoryManagementTestFile.txt";
		std::filesystem::path fileNamePath = fs::path(filename);
		std::ofstream file(filename);
		for (size_t i = 0; i < 50; i++) {
			file << i << " block\n";
		}
		file.close();

		request.append<InstructionType>(1);
		request.append<FileOffset>(2);
		request.append<FileDataLength>(5);  // Request 10 bytes

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 1);
		Message response = ServiceTests::get(0);
		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);

		Filesystem::Path responsePath = "";
		Filesystem::readFullPath(response, responsePath);

		CHECK(responsePath == path);

		CHECK(response.read<InstructionType>() == 1);
		CHECK(response.read<FileOffset>() == 2);
		CHECK(response.read<FileDataLength>() == 5);
		etl::array<uint8_t, 5> data = {};
		response.readString(data.data(), 5);
		String<5> dataString = String<5>(data.data(), 5);
		CHECK(strcmp(dataString.data(), "block") == 0);
		ServiceTests::reset();
	}

	SECTION("Multiple block dump") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "memoryManagementTestFile.txt";
		request.appendOctetString(path);

		request.append<InstructionType>(2);

		// First block
		request.append<FileOffset>(2);
		request.append<FileDataLength>(5);

		// Second block
		request.append<FileOffset>(10);
		request.append<FileDataLength>(5);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::hasOneMessage());
		Message response = ServiceTests::get(0);
		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);

		// Verify response contains both blocks
		Filesystem::Path responsePath = "";
		Filesystem::readFullPath(response, responsePath);
		CHECK(responsePath == path);

		CHECK(response.read<InstructionType>() == 2);

		// First block verification
		CHECK(response.read<FileOffset>() == 2);
		CHECK(response.read<FileDataLength>() == 5);
		etl::array<uint8_t, 5> data1 = {};
		response.readString(data1.data(), 5);
		String<5> dataString1 = String<5>(data1.data(), 5);
		CHECK(strcmp(dataString1.data(), "block") == 0);

		// Second block verification
		CHECK(response.read<FileOffset>() == 10);
		CHECK(response.read<FileDataLength>() == 5);
		etl::array<uint8_t, 5> data2 = {};
		response.readString(data2.data(), 5);
		String<5> dataString2 = String<5>(data2.data(), 5);
		CHECK(strcmp(dataString2.data(), "block") == 0);
		ServiceTests::reset();

	}

	SECTION("File not found error") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "/nonexistent/file.txt";
		request.appendOctetString(path);

		request.append<InstructionType>(1);
		request.append<FileOffset>(0);
		request.append<FileDataLength>(10);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 2);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MemoryObjectDoesNotExist) == 1);

		// Verify error response contains zeros
		Message response = ServiceTests::get(1);
		Filesystem::Path responsePath = "";
		Filesystem::readFullPath(response, responsePath);
		CHECK(responsePath == path);

		CHECK(response.read<InstructionType>() == 1);
		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
		CHECK(response.read<FileOffset>() == 0);
		CHECK(response.read<FileDataLength>() == 0);
		ServiceTests::reset();
	}

	SECTION("Invalid offset error") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "memoryManagementTestFile.txt";
		request.appendOctetString(path);

		request.append<InstructionType>(1);
		request.append<FileOffset>(0xFFFF);
		request.append<FileDataLength>(10);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 2);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::InvalidMemoryOffset) == 1);

		Message response = ServiceTests::get(1);
		Filesystem::Path responsePath = "";
		Filesystem::readFullPath(response, responsePath);
		CHECK(responsePath == path);

		CHECK(response.read<InstructionType>() == 1);
		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
		CHECK(response.read<FileOffset>() == 0xFFFF);
		CHECK(response.read<FileDataLength>() == 0);
		ServiceTests::reset();
	}

	SECTION("Invalid offset error in the middle of multiple instructions") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "memoryManagementTestFile.txt";
		request.appendOctetString(path);

		request.append<InstructionType>(3);

		request.append<FileOffset>(2);
		request.append<FileDataLength>(5);

		request.append<FileOffset>(0xFFFF);
		request.append<FileDataLength>(10);

		request.append<FileOffset>(10);
		request.append<FileDataLength>(5);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::count() == 2);

		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::InvalidMemoryOffset) == 1);

		Message response = ServiceTests::get(1);
		Filesystem::Path responsePath = "";
		Filesystem::readFullPath(response, responsePath);
		CHECK(responsePath == path);

		CHECK(response.read<InstructionType>() == 3);
		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
		CHECK(response.read<FileOffset>() == 2);
		CHECK(response.read<FileDataLength>() == 5);
		etl::array<uint8_t, 5> data1 = {};
		response.readString(data1.data(), 5);
		String<5> dataString1 = String<5>(data1.data(), 5);
		CHECK(strcmp(dataString1.data(), "block") == 0);
		CHECK(response.read<FileOffset>() == 0xFFFF);
		CHECK(response.read<FileDataLength>() == 0);
		CHECK(response.read<FileOffset>() == 10);
		CHECK(response.read<FileDataLength>() == 5);
		etl::array<uint8_t, 5> data2 = {};
		response.readString(data2.data(), 5);
		String<5> dataString2 = String<5>(data2.data(), 5);
		CHECK(strcmp(dataString1.data(), "block") == 0);
		ServiceTests::reset();
	}

	SECTION("Read error") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);

		String<FullPathSize> path = "memoryManagementTestBroken.txt";
		request.appendOctetString(path);

		request.append<InstructionType>(1);
		request.append<FileOffset>(0);
		request.append<FileDataLength>(10);

		namespace fs = std::filesystem;

		const char* filename = "memoryManagementTestBroken.txt";
		std::filesystem::path fileNamePath = fs::path(filename);
		std::ofstream file(filename);
		for (size_t i = 0; i < 50; i++) {
			file << i << " block\n";
		}
		file.close();

#ifdef WIN32
		std::filesystem::permissions(fileNamePath,
			std::filesystem::perms::none);
#else
		chmod(filename, 0);
#endif
		MessageParser::execute(request);

		CHECK(ServiceTests::count() == 2);
		CHECK(ServiceTests::countThrownErrors(ErrorHandler::ExecutionStartErrorType::MemoryReadError) == 1);

		Message response = ServiceTests::get(1);
		Filesystem::Path responsePath = "";
		Filesystem::readFullPath(response, responsePath);
		CHECK(responsePath == path);

		CHECK(response.read<InstructionType>() == 1);
		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
		CHECK(response.read<FileOffset>() == 0);
		CHECK(response.read<FileDataLength>() == 0);
		ServiceTests::reset();
	}
}