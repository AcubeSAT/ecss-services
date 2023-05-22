#include <Message.hpp>
#include <ServicePool.hpp>
#include <catch2/catch_all.hpp>
#include "Services/EventReportService.hpp"
#include "etl/String.hpp"

TEST_CASE("Message is usable", "[message]") {
	Message message(EventReportService::ServiceType, 17, Message::TC, 3);

	REQUIRE(message.serviceType == EventReportService::ServiceType);
	REQUIRE(message.messageType == 17);
	REQUIRE(message.packetType == Message::TC);
	REQUIRE(message.applicationId == 3);
	REQUIRE(message.dataSize == 0);

	message.appendByte(250);
	REQUIRE(message.dataSize == 1);
	REQUIRE(message.readByte() == 250);
}

TEST_CASE("Bit manipulations", "[message]") {
	Message message(0, 0, Message::TC, 0);

	message.appendBits(10, 0x357);
	message.appendBits(4, 0xb);
	message.appendBits(2, 0);
	message.appendByte(248);
	message.appendBits(7, 0x16);
	message.appendBits(1, 0x1);
	message.appendBits(8, 0xff);

	REQUIRE(message.dataSize == 5);

	CHECK(message.readBits(10) == 0x357);
	CHECK(message.readBits(4) == 0xb);
	CHECK(message.readBits(2) == 0);
	CHECK(message.readBits(5) == 0x1f);
	CHECK(message.readBits(3) == 0);
	CHECK(message.readByte() == 0x2d);
	CHECK(message.readByte() == 0xff);

	message.resetRead();

	CHECK(message.readUint32() == 0xd5ecf82d);
	CHECK(message.readBits(8) == 0xff);
}

TEST_CASE("Requirement 5.3.1", "[message][ecss]") {
	SECTION("5.3.1a") {}

	SECTION("5.3.1b") {
		REQUIRE(sizeof(Message::serviceType) == 1);
	}

	SECTION("5.3.1c") {
		// TODO: Unimplemented
	}

	SECTION("5.3.1d") {
		// TODO: Unimplemented
	}
}

TEST_CASE("Requirement 7.3.2 (Boolean)", "[message][ecss]") {
	Message message(0, 0, Message::TC, 0);

	message.append<bool>(false);
	message.append<bool>(true);

	REQUIRE(message.dataSize == 2);

	CHECK_FALSE(message.read<bool>());
	CHECK(message.read<bool>());
}

TEST_CASE("Requirement 7.3.3 (Enumerated)", "[message][ecss]") {
	Message message(0, 0, Message::TC, 0);

	message.appendEnum8(230);
	message.appendEnum16(15933);
	message.appendEnum32(2000001);
	message.appendEnumerated(12, 2052);
	message.appendEnumerated(4, 10);

	REQUIRE(message.dataSize == 1 + 2 + 4 + 2);

	CHECK(message.readEnum8() == 230);
	CHECK(message.readEnum16() == 15933);
	CHECK(message.readEnum32() == 2000001);
	CHECK(message.readEnumerated(12) == 2052);
	CHECK(message.readEnumerated(4) == 10);
}

TEST_CASE("Requirement 7.3.4 (Unsigned integer)", "[message][ecss]") {
	Message message(0, 0, Message::TC, 0);

	message.append<uint8_t>(230);
	message.append<uint16_t>(15933);
	message.append<uint32_t>(2000001);
	message.append<uint64_t>(12446744073709551615ULL);

	REQUIRE(message.dataSize == 1 + 2 + 4 + 8);

	CHECK(message.read<uint8_t>() == 230);
	CHECK(message.read<uint16_t>() == 15933);
	CHECK(message.read<uint32_t>() == 2000001);
	CHECK(message.read<uint64_t>() == 12446744073709551615ULL);

	SECTION("7.4.3") {
		/**
		 * Make sure the endianness of the message data is correct.
		 * As per the ECSS standard, stored data should be big-endian. However, ARM and x86
		 * processors store data in little endian format. As a result, special care needs to be
		 * taken for compliance.
		 */
		CHECK(message.data[1] == 0x3e);
		CHECK(message.data[2] == 0x3d);
	}
}

TEST_CASE("Requirement 7.3.5 (Signed integer)", "[message][ecss]") {
	Message message(0, 0, Message::TC, 0);

	message.append<int8_t>(-16);
	message.append<int16_t>(-7009);
	message.append<int32_t>(-2000001);
	message.append<int32_t>(15839011);

	REQUIRE(message.dataSize == 1 + 2 + 4 + 4);

	CHECK(message.read<int8_t>() == -16);
	CHECK(message.read<int16_t>() == -7009);
	CHECK(message.read<int32_t>() == -2000001);
	CHECK(message.read<int32_t>() == 15839011);

	SECTION("7.4.3") {
		// Make sure the endianness of the message data is correct
		// As per the ECSS standard, stored data should be big-endian. However, ARM and x86
		// processors store data in little endian format. As a result, special care needs to be
		// taken for compliance.
		CHECK(message.data[1] == 0xe4);
		CHECK(message.data[2] == 0x9f);
	}
}

TEST_CASE("Requirement 7.3.6 (Real)", "[message][ecss]") {
	Message message(0, 0, Message::TC, 0);

	message.append<float>(7.209f);
	message.append<float>(-9003.53135f);

	REQUIRE(message.dataSize == 8);

	CHECK(message.read<float>() == 7.209f);
	CHECK(message.read<float>() == -9003.53135f);
}

TEST_CASE("Test appending double") {
	Message message(0, 0, Message::TC, 0);
	message.append<double>(2.324);

	REQUIRE(message.dataSize == 8);

	CHECK(message.read<double>() == Catch::Approx(2.324).epsilon(0.0001));
}

TEST_CASE("Test appending offset") {
	Message message(0, 0, Message::TC, 0);
	message.append<Time::RelativeTime>(555);

	REQUIRE(message.dataSize == 8);

	CHECK(message.read<Time::RelativeTime>() == 555);
}

TEST_CASE("Test appending a CUC timestamp") {
	using namespace Time;

	SECTION("Test 1") {
		auto timeCUC = TimeGetter::getCurrentTimeDefaultCUC();
		REQUIRE(timeCUC.formatAsBytes() == 86769000);

		Message message(0, 0, Message::TC, 0);
		message.appendDefaultCUCTimeStamp(timeCUC);

		REQUIRE(message.readUint32() == 86769000);
	}

	SECTION("Test 2") {
		DefaultCUC timeCUC(34511_t);

		Message message(0, 0, Message::TC, 0);
		message.appendDefaultCUCTimeStamp(timeCUC);

		REQUIRE(message.readUint32() == 34511);
	}
}

TEST_CASE("Test reading a custom CUC timestamp") {
	using namespace Time;
	/**
 	* Append a custom CUC Time Stamp to a message object and check if is it read corretly
 	*/
	DefaultCUC timeCUC(34511_t);

	Message message(0, 0, Message::TC, 0);
	message.appendDefaultCUCTimeStamp(timeCUC);

	auto returnTimeCUC = message.readDefaultCUCTimeStamp();

	REQUIRE(returnTimeCUC.formatAsBytes() == 34511);
}

TEST_CASE("Requirement 7.3.8 (Octet-string)", "[message][ecss]") {
	Message message(0, 0, Message::TC, 0);

	message.appendString(String<4>("test"));
	message.append<etl::istring>(String<4>("gaus"));

	REQUIRE(message.dataSize == 4 + 6);

	char string[5];
	message.readCString(string, 4);
	CHECK_THAT(string, Catch::Matchers::Equals("test"));

	auto output = message.readOctetString<10>();
	CHECK_THAT(output.c_str(), Catch::Matchers::Equals("gaus"));
}

TEST_CASE("Requirement 7.3.13 (Packet)", "[message][ecss]") {
	Message telemetry(0, 0, Message::TM, 0);
	Message telecommand(0, 0, Message::TC, 0);

	CHECK(static_cast<int>(telemetry.packetType) == 0);
	CHECK(static_cast<int>(telecommand.packetType) == 1);
}

TEST_CASE("Spare field", "[message]") {
	Message message1(0, 0, Message::TM, 0);

	message1.appendByte(1);
	message1.appendHalfword(2);
	message1.appendBits(1, 1);
	message1.finalize();

	CHECK(message1.data[3] == 0b10000000);
	CHECK(message1.dataSize == 4);

	Message message2(0, 0, Message::TM, 0);
	message2.appendByte(1);
	message2.appendHalfword(2);
	message2.appendBits(2, 3);
	message2.finalize();

	CHECK(message2.data[3] == 0b11000000);
	CHECK(message2.dataSize == 4);

	Message message3(0, 0, Message::TM, 0);

	message3.appendByte(1);
	message3.appendHalfword(2);
	message3.appendBits(3, 5);
	message3.finalize();

	CHECK(message3.data[3] == 0b10100000);
	CHECK(message3.dataSize == 4);

	Message message4(0, 0, Message::TM, 0);

	message4.appendByte(1);
	message4.appendHalfword(2);
	message4.appendBits(4, 5);
	message4.finalize();

	CHECK(message4.data[3] == 0b01010000);
	CHECK(message4.dataSize == 4);

	Message message5(0, 0, Message::TM, 0);

	message5.appendByte(1);
	message5.appendHalfword(2);
	message5.appendBits(5, 5);
	message5.finalize();

	CHECK(message5.data[3] == 0b00101000);
	CHECK(message5.dataSize == 4);

	Message message6(0, 0, Message::TM, 0);

	message6.appendByte(1);
	message6.appendHalfword(2);
	message6.appendBits(6, 5);
	message6.finalize();

	CHECK(message6.data[3] == 0b00010100);
	CHECK(message6.dataSize == 4);

	Message message7(0, 0, Message::TM, 0);

	message7.appendByte(1);
	message7.appendHalfword(2);
	message7.appendBits(7, 5);
	message7.finalize();

	CHECK(message7.data[3] == 0b00001010);
	CHECK(message7.dataSize == 4);

	Message message8(0, 0, Message::TM, 0);

	message8.appendByte(1);
	message8.appendHalfword(2);
	message8.appendBits(8, 5);
	message8.finalize();

	CHECK(message8.data[3] == 0b00000101);
	CHECK(message8.dataSize == 4);

	Message message9(0, 0, Message::TM, 0);

	message9.appendByte(1);
	message9.appendHalfword(2);
	message9.appendBits(0, 5);
	message9.finalize();

	CHECK(message9.dataSize == 3);
}

TEST_CASE("Message type counter", "[message]") {
	SECTION("Message counting") {
		Message message1(0, 0, Message::TM, 0);
		message1.finalize();
		CHECK(message1.messageTypeCounter == 0);

		Message message2(0, 0, Message::TM, 0);
		message2.finalize();
		CHECK(message2.messageTypeCounter == 1);
	}

	SECTION("Different message types") {
		Message message1(0, 1, Message::TM, 0);
		message1.finalize();
		CHECK(message1.messageTypeCounter == 0);

		Message message2(0, 2, Message::TM, 0);
		message2.finalize();
		CHECK(message2.messageTypeCounter == 0);
	}

	SECTION("Message counter overflow") {
		for (int i = 0; i <= 65534; i++) {
			Message message(0, 3, Message::TM, 0);
			message.finalize();
		}

		Message message1(0, 3, Message::TM, 0);
		message1.finalize();
		CHECK(message1.messageTypeCounter == 65535);

		Message message2(0, 3, Message::TM, 0);
		message2.finalize();
		CHECK(message2.messageTypeCounter == 0);
	}
}

TEST_CASE("Packet sequence counter", "[message]") {
	SECTION("Packet counting") {
		Message message1(0, 0, Message::TM, 0);
		message1.finalize();
		CHECK(message1.packetSequenceCount == 0);

		Message message2(0, 0, Message::TM, 0);
		message2.finalize();
		CHECK(message2.packetSequenceCount == 1);

		// Different message type check
		Message message3(1, 2, Message::TM, 0);
		message3.finalize();
		CHECK(message3.packetSequenceCount == 2);
	}

	SECTION("Packet counter overflow") {
		Services.reset();

		for (int i = 0; i <= 16382; i++) {
			Message message(0, 3, Message::TM, 0);
			message.finalize();
		}

		Message message1(0, 3, Message::TM, 0);
		message1.finalize();
		CHECK(message1.packetSequenceCount == 16383);

		Message message2(0, 3, Message::TM, 0);
		message2.finalize();
		CHECK(message2.packetSequenceCount == 0);
	}
}

TEST_CASE("Storing and retrieving enums in Messages") {
	enum ActiveBus : uint8_t {
		Main = 0x0,
		Redundant = 0x1
	};

	SECTION("Using Enums in parameters") {
		auto parameter1 = Parameter<ActiveBus>(Redundant);
		auto parameter2 = Parameter<ActiveBus>(Main);
		Message message;

		parameter1.appendValueToMessage(message);
		parameter2.setValueFromMessage(message);

		CHECK(parameter1.getValue() == parameter2.getValue());
	}

	SECTION("Using Enums in variables") {
		ActiveBus variable1 = Redundant;
		ActiveBus variable2 = Main;
		Message message;

		message.append(variable1);
		variable2 = message.read<ActiveBus>();

		CHECK(variable1 == variable2);
	}

	SECTION("Another type of enum") {
		enum Numbers : uint64_t {
			First = 1577829600,
			Second = 1667045679
		};

		auto parameter1 = Parameter<Numbers>(First);
		auto parameter2 = Parameter<Numbers>(Second);
		Message message;

		parameter1.appendValueToMessage(message);
		parameter2.setValueFromMessage(message);

		CHECK(parameter1.getValue() == parameter2.getValue());
	}

		SECTION("Another type of enum") {
		enum MemoryPartitionUsed : bool {
			first = false,
			second = true
		};

		auto parameter3 = Parameter<MemoryPartitionUsed>(first);
		auto parameter4 = Parameter<MemoryPartitionUsed>(second);
		Message message;

		parameter3.appendValueToMessage(message);
		parameter4.setValueFromMessage(message);

		CHECK(parameter3.getValue() == parameter4.getValue());
	}
}
