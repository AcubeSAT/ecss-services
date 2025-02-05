#include <Message.hpp>
#include <Services/MemoryManagementService.hpp>
#include <catch2/catch_all.hpp>
#include "Helpers/CRCHelper.hpp"
#include "ServiceTests.hpp"

MemoryManagementService& memMangService = Services.memoryManagement;

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
		
		const Offset offset = 0;
		const String<ChunkMaxFileSizeBytes> testData = "Hello World";
		const FileDataLength dataLength = testData.size();
		
		request.append<Offset>(offset);
		request.append<FileDataLength>(dataLength);
		request.appendString(testData);

		MessageParser::execute(request);
		REQUIRE(ServiceTests::count() == 1);
		// REQUIRE(ServiceTests::hasOneMessage());
		Message response = ServiceTests::get(0);
		CHECK(response.messageType == RequestVerificationService::SuccessfulCompletionOfExecution);
	}

	SECTION("Multiple instruction load") {
		Message request(MemoryManagementService::ServiceType, MemoryManagementService::LoadObjectMemoryData, Message::TC, 1);
		
		String<FullPathSize> path = "memoryManagementTestFile.txt";
		request.appendOctetString(path);
		
		// Two instructions
		request.append<InstructionType>(2);
		
		// First data block
		const Offset offset1 = 0;
		const String<ChunkMaxFileSizeBytes> testData1 = "First block";
		const FileDataLength dataLength1 = testData1.size();
		
		request.append<Offset>(offset1);
		request.append<FileDataLength>(dataLength1);
		request.appendString(testData1);

		// Second data block
		const Offset offset2 = 100;
		const String<ChunkMaxFileSizeBytes> testData2 = "Second block";
		const FileDataLength dataLength2 = testData2.size();
		
		request.append<Offset>(offset2);
		request.append<FileDataLength>(dataLength2);
		request.appendString(testData2);

		MessageParser::execute(request);

		REQUIRE(ServiceTests::hasOneMessage());
		Message response = ServiceTests::get(0);
		CHECK(response.messageType == RequestVerificationService::SuccessfulCompletionOfExecution);
	}

	// SECTION("File not found error") {
	// 	Message request(MemoryManagementService::ServiceType, MemoryManagementService::LoadObjectMemoryData, Message::TC, 1);
	//
	// 	String<FullPathSize> path = "/nonexistent/file.txt";
	// 	request.appendString(path);
	//
	// 	request.append<InstructionType>(1);
	//
	// 	const Offset offset = 0;
	// 	const uint8_t testData[] = "Test data";
	// 	const FileDataLength dataLength = sizeof(testData);
	//
	// 	request.append<Offset>(offset);
	// 	request.append<FileDataLength>(dataLength);
	// 	request.appendString(String<ECSSMaxFixedOctetStringSize>(testData, dataLength));
	//
	// 	MessageParser::execute(request);
	//
	// 	REQUIRE(ServiceTests::count() == 2);
	//
	// 	// Check error report
	// 	Message errorReport = ServiceTests::get(0);
	// 	CHECK(errorReport.messageType == ErrorHandler::MessageType::ErrorReport);
	// 	CHECK(errorReport.read<ErrorHandler::ExecutionStartErrorType>() ==
	// 	ErrorHandler::ExecutionStartErrorType::MemoryObjectDoesNotExist);
	//
	// 	// Check failed completion
	// 	Message completion = ServiceTests::get(1);
	// 	CHECK(completion.messageType == RequestVerificationService::FailedCompletionOfExecution);
	// }
	//
	// SECTION("Invalid offset error") {
	// 	Message request(MemoryManagementService::ServiceType, MemoryManagementService::LoadObjectMemoryData, Message::TC, 1);
	//
	// 	String<FullPathSize> path = "/test/file.txt";
	// 	request.appendString(path);
	//
	// 	request.append<InstructionType>(1);
	//
	// 	const Offset invalidOffset = 0xFFFFFFFF;  // Very large offset
	// 	const uint8_t testData[] = "Test data";
	// 	const FileDataLength dataLength = sizeof(testData);
	//
	// 	request.append<Offset>(invalidOffset);
	// 	request.append<FileDataLength>(dataLength);
	// 	request.appendString(String<ECSSMaxFixedOctetStringSize>(testData, dataLength));
	//
	// 	MessageParser::execute(request);
	//
	// 	REQUIRE(ServiceTests::count() == 2);
	//
	// 	Message errorReport = ServiceTests::get(0);
	// 	CHECK(errorReport.messageType == ErrorHandler::MessageType::ErrorReport);
	// 	CHECK(errorReport.read<ErrorHandler::ExecutionStartErrorType>() == ErrorHandler::ExecutionStartErrorType::InvalidMemoryOffset);
	// }
}

// TEST_CASE("TC[6,3] Dump Object Memory Data", "[service][st06]") {
// 	SECTION("Successful single block dump") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		String<FullPathSize> path = "/test/file.txt";
// 		request.appendString(path);
//
// 		request.append<InstructionType>(1);
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(10);  // Request 10 bytes
//
// 		MessageParser::execute(request);
//
// 		REQUIRE(ServiceTests::hasOneMessage());
// 		Message response = ServiceTests::get(0);
// 		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
//
// 		String<FullPathSize> responsePath = "";
// 		response.readString(responsePath, FullPathSize);
// 		CHECK(responsePath == path);
//
// 		CHECK(response.read<InstructionType>() == 1);
// 		CHECK(response.read<Offset>() == 0);
// 		CHECK(response.read<FileDataLength>() == 10);
//
// 		// Verify data content if needed
// 		String<10> data;
// 		response.readString(data);
// 		// Add specific data checks here
// 	}
//
// 	SECTION("Multiple block dump") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		String<FullPathSize> path = "/test/file.txt";
// 		request.appendString(path);
//
// 		request.append<InstructionType>(2);
//
// 		// First block
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(5);
//
// 		// Second block
// 		request.append<Offset>(10);
// 		request.append<FileDataLength>(5);
//
// 		MessageParser::execute(request);
//
// 		REQUIRE(ServiceTests::hasOneMessage());
// 		Message response = ServiceTests::get(0);
// 		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
//
// 		// Verify response contains both blocks
// 		String<FullPathSize> responsePath;
// 		response.readString(responsePath);
// 		CHECK(responsePath == path);
//
// 		CHECK(response.read<InstructionType>() == 2);
//
// 		// First block verification
// 		CHECK(response.read<Offset>() == 0);
// 		CHECK(response.read<FileDataLength>() == 5);
// 		String<5> data1;
// 		response.readString(data1);
//
// 		// Second block verification
// 		CHECK(response.read<Offset>() == 10);
// 		CHECK(response.read<FileDataLength>() == 5);
// 		String<5> data2;
// 		response.readString(data2);
// 	}
//
// 	SECTION("File not found error") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		String<FullPathSize> path = "/nonexistent/file.txt";
// 		request.appendString(path);
//
// 		request.append<InstructionType>(1);
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(10);
//
// 		MessageParser::execute(request);
//
// 		REQUIRE(ServiceTests::count() == 2);
//
// 		Message errorReport = ServiceTests::get(0);
// 		CHECK(errorReport.messageType == ErrorHandler::MessageType::ErrorReport);
// 		CHECK(errorReport.read<ErrorHandler::ExecutionStartErrorType>() == ErrorHandler::ExecutionStartErrorType::MemoryObjectDoesNotExist);
//
// 		// Verify error response contains zeros
// 		Message response = ServiceTests::get(1);
// 		CHECK(response.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
// 		CHECK(response.read<Offset>() == 0);
// 		CHECK(response.read<FileDataLength>() == 0);
// 	}
// }
//
// TEST_CASE("TM[6,4] Dumped Object Memory Data Report", "[service][st06]") {
// 	SECTION("Single block report") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		// Setup test file with known content
// 		const String<FullPathSize> path = "/test/report.txt";
// 		const uint8_t testData[] = "Test file content";
// 		const FileDataLength contentLength = sizeof(testData);
//
// 		// Create test file
// 		auto writeResult = Filesystem::writeFile(path, 0, contentLength, testData);
// 		REQUIRE(!writeResult.has_value());
//
// 		// Build request
// 		request.appendString(path);
// 		request.append<InstructionType>(1);
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(contentLength);
//
// 		MessageParser::execute(request);
//
// 		REQUIRE(ServiceTests::hasOneMessage());
// 		Message report = ServiceTests::get(0);
//
// 		// Verify report format
// 		CHECK(report.serviceType == MemoryManagementService::ServiceType);
// 		CHECK(report.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
//
// 		// Verify path
// 		String<FullPathSize> reportedPath;
// 		report.readString(reportedPath);
// 		CHECK(reportedPath == path);
//
// 		// Verify instruction count
// 		CHECK(report.read<InstructionType>() == 1);
//
// 		// Verify data block
// 		CHECK(report.read<Offset>() == 0);
// 		CHECK(report.read<FileDataLength>() == contentLength);
//
// 		String<sizeof(testData)> reportedData;
// 		report.readString(reportedData);
// 		CHECK(memcmp(reportedData.data(), testData, contentLength) == 0);
// 	}
//
// 	SECTION("Report with invalid offset") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		const String<FullPathSize> path = "/test/report.txt";
// 		request.appendString(path);
// 		request.append<InstructionType>(1);
// 		request.append<Offset>(0xFFFFFFFF); // Invalid offset
// 		request.append<FileDataLength>(10);
//
// 		MessageParser::execute(request);
//
// 		REQUIRE(ServiceTests::count() == 2);
//
// 		// Verify error report
// 		Message errorReport = ServiceTests::get(0);
// 		CHECK(errorReport.messageType == ErrorHandler::MessageType::ErrorReport);
// 		CHECK(errorReport.read<ErrorHandler::ExecutionStartErrorType>() == ErrorHandler::ExecutionStartErrorType::InvalidMemoryOffset);
//
// 		// Verify error response format
// 		Message report = ServiceTests::get(1);
// 		CHECK(report.serviceType == MemoryManagementService::ServiceType);
// 		CHECK(report.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
//
// 		String<FullPathSize> reportedPath;
// 		report.readString(reportedPath);
// 		CHECK(reportedPath == path);
// 		CHECK(report.read<InstructionType>() == 1);
// 		CHECK(report.read<Offset>() == 0);
// 		CHECK(report.read<FileDataLength>() == 0);
// 	}
//
// 	SECTION("Report with invalid buffer size") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		const String<FullPathSize> path = "/test/report.txt";
// 		request.appendString(path);
// 		request.append<InstructionType>(1);
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(0xFFFF); // Too large buffer size
//
// 		MessageParser::execute(request);
//
// 		REQUIRE(ServiceTests::count() == 2);
//
// 		// Verify error report
// 		Message errorReport = ServiceTests::get(0);
// 		CHECK(errorReport.messageType == ErrorHandler::MessageType::ErrorReport);
// 		CHECK(errorReport.read<ErrorHandler::ExecutionStartErrorType>() ==
// 		ErrorHandler::ExecutionStartErrorType::MemoryBufferSizeError);
//
// 		// Verify error response format
// 		Message report = ServiceTests::get(1);
// 		CHECK(report.serviceType == MemoryManagementService::ServiceType);
// 		CHECK(report.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
//
// 		String<FullPathSize> reportedPath;
// 		report.readString(reportedPath);
// 		CHECK(reportedPath == path);
// 		CHECK(report.read<InstructionType>() == 1);
// 		CHECK(report.read<Offset>() == 0);
// 		CHECK(report.read<FileDataLength>() == 0);
// 	}
//
// 	SECTION("Report with read error") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		const String<FullPathSize> path = "/test/report.txt";
// 		request.appendString(path);
// 		request.append<InstructionType>(1);
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(10);
//
// 		// Mock a read error in filesystem
// 		Filesystem::mockReadError = Filesystem::FileWriteError::WriteError;
// 		MessageParser::execute(request);
// 		Filesystem::mockReadError = std::nullopt;
//
// 		REQUIRE(ServiceTests::count() == 2);
//
// 		// Verify error report
// 		Message errorReport = ServiceTests::get(0);
// 		CHECK(errorReport.messageType == ErrorHandler::MessageType::ErrorReport);
// 		CHECK(errorReport.read<ErrorHandler::ExecutionStartErrorType>() == ErrorHandler::ExecutionStartErrorType::MemoryReadError);
//
// 		// Verify error response format
// 		Message report = ServiceTests::get(1);
// 		CHECK(report.serviceType == MemoryManagementService::ServiceType);
// 		CHECK(report.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
// 		CHECK(report.read<Offset>() == 0);
// 		CHECK(report.read<FileDataLength>() == 0);
// 	}
//
// 	SECTION("Report with unknown error") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		const String<FullPathSize> path = "/test/report.txt";
// 		request.appendString(path);
// 		request.append<InstructionType>(1);
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(10);
//
// 		// Mock an unknown error in filesystem
// 		Filesystem::mockReadError = Filesystem::FileWriteError::UnknownError;
// 		MessageParser::execute(request);
// 		Filesystem::mockReadError = std::nullopt;
//
// 		REQUIRE(ServiceTests::count() == 2);
//
// 		// Verify error report
// 		Message errorReport = ServiceTests::get(0);
// 		CHECK(errorReport.messageType == ErrorHandler::MessageType::ErrorReport);
// 		CHECK(errorReport.read<ErrorHandler::ExecutionStartErrorType>() ==
// 		ErrorHandler::ExecutionStartErrorType::UnknownMemoryReadError);
//
// 		// Verify error response format
// 		Message report = ServiceTests::get(1);
// 		CHECK(report.serviceType == MemoryManagementService::ServiceType);
// 		CHECK(report.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
// 		CHECK(report.read<Offset>() == 0);
// 		CHECK(report.read<FileDataLength>() == 0);
// 	}
//
// 	SECTION("Multiple blocks with error in second block") {
// 		Message request(MemoryManagementService::ServiceType, MemoryManagementService::DumpObjectMemoryData, Message::TC, 1);
//
// 		// Setup test file with known content
// 		const String<FullPathSize> path = "/test/report.txt";
// 		const uint8_t testData[] = "Test content for multiple blocks";
// 		const FileDataLength contentLength = sizeof(testData);
//
// 		// Create test file
// 		auto writeResult = Filesystem::writeFile(path, 0, contentLength, testData);
// 		REQUIRE(!writeResult.has_value());
//
// 		request.appendString(path);
// 		request.append<InstructionType>(2);
//
// 		// First block should succeed
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(10);
//
// 		// Second block with invalid offset
// 		request.append<Offset>(0xFFFFFFFF);
// 		request.append<FileDataLength>(10);
//
// 		MessageParser::execute(request);
//
// 		REQUIRE(ServiceTests::count() == 2);
//
// 		// Verify error report
// 		Message errorReport = ServiceTests::get(0);
// 		CHECK(errorReport.messageType == ErrorHandler::MessageType::ErrorReport);
// 		CHECK(errorReport.read<ErrorHandler::ExecutionStartErrorType>() ==
// 		ErrorHandler::ExecutionStartErrorType::InvalidMemoryOffset);
//
// 		// Verify report format - should contain first block but zeros for second
// 		Message report = ServiceTests::get(1);
// 		CHECK(report.serviceType == MemoryManagementService::ServiceType);
// 		CHECK(report.messageType == MemoryManagementService::DumpedObjectMemoryDataReport);
//
// 		String<FullPathSize> reportedPath;
// 		report.readString(reportedPath);
// 		CHECK(reportedPath == path);
// 		CHECK(report.read<InstructionType>() == 2);
//
// 		// First block should be valid
// 		CHECK(report.read<Offset>() == 0);
// 		CHECK(report.read<FileDataLength>() == 10);
// 		String<10> data1;
// 		report.readString(data1);
// 		CHECK(memcmp(data1.data(), testData, 10) == 0);
//
// 		// Second block should be zeros
// 		CHECK(report.read<Offset>() == 0);
// 		CHECK(report.read<FileDataLength>() == 0);
// 	}
//
// 	SECTION("Invalid message type") {
// 		Message request(MemoryManagementService::ServiceType,
// 					   static_cast<Message::MessageType>(0xFF), // Invalid message type
// 					   Message::TC, 1);
//
// 		const String<FullPathSize> path = "/test/report.txt";
// 		request.appendString(path);
// 		request.append<InstructionType>(1);
// 		request.append<Offset>(0);
// 		request.append<FileDataLength>(10);
//
// 		MessageParser::execute(request);
//
// 		// Should be no response for invalid message type
// 		CHECK(ServiceTests::count() == 0);
// 	}
// }
