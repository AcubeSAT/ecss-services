#include <ECSSMessage.hpp>
#include <ServicePool.hpp>
#include <catch2/catch_all.hpp>
#include "Services/EventReportService.hpp"
#include "etl/String.hpp"

TEST_CASE("ECSSMessage is usable", "[message]") {
	ECSSMessage message(EventReportService::ServiceType, 17, ECSSMessage::TC, 3);

	REQUIRE(message.serviceType == EventReportService::ServiceType);
	REQUIRE(message.messageType == 17);
	REQUIRE(message.packetType == ECSSMessage::TC);
	REQUIRE(message.applicationId == 3);
	REQUIRE(message.dataSize == 0);

	message.appendByte(250);
	REQUIRE(message.dataSize == 1);
	REQUIRE(message.readByte() == 250);
}

TEST_CASE("Bit manipulations", "[message]") {
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);

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
		REQUIRE(sizeof(ECSSMessage::serviceType) == 1);
	}

	SECTION("5.3.1c") {
		// TODO: Unimplemented
	}

	SECTION("5.3.1d") {
		// TODO: Unimplemented
	}
}

TEST_CASE("Requirement 7.3.2 (Boolean)", "[message][ecss]") {
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);

	message.append<bool>(false);
	message.append<bool>(true);

	REQUIRE(message.dataSize == 2);

	CHECK_FALSE(message.read<bool>());
	CHECK(message.read<bool>());
}

TEST_CASE("Requirement 7.3.3 (Enumerated)", "[message][ecss]") {
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);

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
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);

	message.append<char>(110);
	message.append<uint8_t>(230);
	message.append<uint16_t>(15933);
	message.append<uint32_t>(2000001);
	message.append<uint64_t>(12446744073709551615ULL);

	REQUIRE(message.dataSize == 1 + 1 + 2 + 4 + 8);

	CHECK(message.read<char>() == 110);
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
		CHECK(message.data[2] == 0x3e);
		CHECK(message.data[3] == 0x3d);
	}
}

TEST_CASE("Requirement 7.3.5 (Signed integer)", "[message][ecss]") {
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);

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
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);

	message.append<float>(7.209f);
	message.append<float>(-9003.53135f);

	REQUIRE(message.dataSize == 8);

	CHECK(message.read<float>() == 7.209f);
	CHECK(message.read<float>() == -9003.53135f);
}

TEST_CASE("Test appending double") {
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);
	message.append<double>(2.324);

	REQUIRE(message.dataSize == 8);

	CHECK(message.read<double>() == Catch::Approx(2.324).epsilon(0.0001));
}

TEST_CASE("Test appending offset") {
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);
	message.append<Time::RelativeTime>(555);

	REQUIRE(message.dataSize == 8);

	CHECK(message.read<Time::RelativeTime>() == 555);
}

TEST_CASE("Test appending a CUC timestamp") {
	using namespace Time;

	SECTION("Test 1") {
		auto timeCUC = TimeGetter::getCurrentTimeDefaultCUC();
		REQUIRE(timeCUC.formatAsBytes() == 86769000);

		ECSSMessage message(0, 0, ECSSMessage::TC, 0);
		message.appendDefaultCUCTimeStamp(timeCUC);

		REQUIRE(message.readUint32() == 86769000);
	}

	SECTION("Test 2") {
		DefaultCUC timeCUC(34511_t);

		ECSSMessage message(0, 0, ECSSMessage::TC, 0);
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

	ECSSMessage message(0, 0, ECSSMessage::TC, 0);
	message.appendDefaultCUCTimeStamp(timeCUC);

	auto returnTimeCUC = message.readDefaultCUCTimeStamp();

	REQUIRE(returnTimeCUC.formatAsBytes() == 34511);
}

TEST_CASE("Requirement 7.3.8 (Octet-string)", "[message][ecss]") {
	ECSSMessage message(0, 0, ECSSMessage::TC, 0);

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
	ECSSMessage telemetry(0, 0, ECSSMessage::TM, 0);
	ECSSMessage telecommand(0, 0, ECSSMessage::TC, 0);

	CHECK(static_cast<int>(telemetry.packetType) == 0);
	CHECK(static_cast<int>(telecommand.packetType) == 1);
}

TEST_CASE("Spare field", "[message]") {
	ECSSMessage message1(0, 0, ECSSMessage::TM, 0);

	message1.appendByte(1);
	message1.appendHalfword(2);
	message1.appendBits(1, 1);
	message1.finalize();

	CHECK(message1.data[3] == 0b10000000);
	CHECK(message1.dataSize == 4);

	ECSSMessage message2(0, 0, ECSSMessage::TM, 0);
	message2.appendByte(1);
	message2.appendHalfword(2);
	message2.appendBits(2, 3);
	message2.finalize();

	CHECK(message2.data[3] == 0b11000000);
	CHECK(message2.dataSize == 4);

	ECSSMessage message3(0, 0, ECSSMessage::TM, 0);

	message3.appendByte(1);
	message3.appendHalfword(2);
	message3.appendBits(3, 5);
	message3.finalize();

	CHECK(message3.data[3] == 0b10100000);
	CHECK(message3.dataSize == 4);

	ECSSMessage message4(0, 0, ECSSMessage::TM, 0);

	message4.appendByte(1);
	message4.appendHalfword(2);
	message4.appendBits(4, 5);
	message4.finalize();

	CHECK(message4.data[3] == 0b01010000);
	CHECK(message4.dataSize == 4);

	ECSSMessage message5(0, 0, ECSSMessage::TM, 0);

	message5.appendByte(1);
	message5.appendHalfword(2);
	message5.appendBits(5, 5);
	message5.finalize();

	CHECK(message5.data[3] == 0b00101000);
	CHECK(message5.dataSize == 4);

	ECSSMessage message6(0, 0, ECSSMessage::TM, 0);

	message6.appendByte(1);
	message6.appendHalfword(2);
	message6.appendBits(6, 5);
	message6.finalize();

	CHECK(message6.data[3] == 0b00010100);
	CHECK(message6.dataSize == 4);

	ECSSMessage message7(0, 0, ECSSMessage::TM, 0);

	message7.appendByte(1);
	message7.appendHalfword(2);
	message7.appendBits(7, 5);
	message7.finalize();

	CHECK(message7.data[3] == 0b00001010);
	CHECK(message7.dataSize == 4);

	ECSSMessage message8(0, 0, ECSSMessage::TM, 0);

	message8.appendByte(1);
	message8.appendHalfword(2);
	message8.appendBits(8, 5);
	message8.finalize();

	CHECK(message8.data[3] == 0b00000101);
	CHECK(message8.dataSize == 4);

	ECSSMessage message9(0, 0, ECSSMessage::TM, 0);

	message9.appendByte(1);
	message9.appendHalfword(2);
	message9.appendBits(0, 5);
	message9.finalize();

	CHECK(message9.dataSize == 3);
}

TEST_CASE("ECSSMessage type counter", "[message]") {
	SECTION("ECSSMessage counting") {
		ECSSMessage message1(0, 0, ECSSMessage::TM, 0);
		message1.finalize();
		CHECK(message1.messageTypeCounter == 0);

		ECSSMessage message2(0, 0, ECSSMessage::TM, 0);
		message2.finalize();
		CHECK(message2.messageTypeCounter == 1);
	}

	SECTION("Different message types") {
		ECSSMessage message1(0, 1, ECSSMessage::TM, 0);
		message1.finalize();
		CHECK(message1.messageTypeCounter == 0);

		ECSSMessage message2(0, 2, ECSSMessage::TM, 0);
		message2.finalize();
		CHECK(message2.messageTypeCounter == 0);
	}

	SECTION("ECSSMessage counter overflow") {
		for (int i = 0; i <= 65534; i++) {
			ECSSMessage message(0, 3, ECSSMessage::TM, 0);
			message.finalize();
		}

		ECSSMessage message1(0, 3, ECSSMessage::TM, 0);
		message1.finalize();
		CHECK(message1.messageTypeCounter == 65535);

		ECSSMessage message2(0, 3, ECSSMessage::TM, 0);
		message2.finalize();
		CHECK(message2.messageTypeCounter == 0);
	}
}

TEST_CASE("Packet sequence counter", "[message]") {
	SECTION("Packet counting") {
		ECSSMessage message1(0, 0, ECSSMessage::TM, 0);
		message1.finalize();
		CHECK(message1.packetSequenceCount == 0);

		ECSSMessage message2(0, 0, ECSSMessage::TM, 0);
		message2.finalize();
		CHECK(message2.packetSequenceCount == 1);

		// Different message type check
		ECSSMessage message3(1, 2, ECSSMessage::TM, 0);
		message3.finalize();
		CHECK(message3.packetSequenceCount == 2);
	}

	SECTION("Packet counter overflow") {
		Services.reset();

		for (int i = 0; i <= 16382; i++) {
			ECSSMessage message(0, 3, ECSSMessage::TM, 0);
			message.finalize();
		}

		ECSSMessage message1(0, 3, ECSSMessage::TM, 0);
		message1.finalize();
		CHECK(message1.packetSequenceCount == 16383);

		ECSSMessage message2(0, 3, ECSSMessage::TM, 0);
		message2.finalize();
		CHECK(message2.packetSequenceCount == 0);
	}
}
