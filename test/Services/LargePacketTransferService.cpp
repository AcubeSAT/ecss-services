#include <catch2/catch.hpp>
#include <Services/LargePacketTransferService.hpp>
#include <Message.hpp>
#include "ServiceTests.hpp"
#include <cstring>
#include <etl/String.hpp>

LargePacketTransferService &lPT = Services.largePacketTransferService;

TEST_CASE("First Downlink Part Report TM[13,1]", "[service][st13]") {
	String<ECSS_MAX_FIXED_OCTET_STRING_SIZE> string = String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>
		("12345678");
	lPT.firstDownlinkPartReport(1, 1, string);
	REQUIRE(ServiceTests::hasOneMessage());
	Message report = ServiceTests::get(0);
	CHECK(report.messageType == 1);
	CHECK(report.serviceType == 13);
	CHECK(report.readUint16() == 1);
	CHECK(report.readUint16() == 1);
	uint8_t string2[ECSS_MAX_FIXED_OCTET_STRING_SIZE];
	printf("%d", report.readOctetString(string2));
	auto a = String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>(string2, 8);
	CHECK(string.compare(a) == 0);
}

TEST_CASE("Intermediate Downlink Part Report TM[13,2]", "[service][st13]") {
	String<ECSS_MAX_FIXED_OCTET_STRING_SIZE> string = String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>(
		"12345678");
	lPT.intermediateDownlinkPartReport(1, 1, string);
	REQUIRE(ServiceTests::hasOneMessage());
	Message report = ServiceTests::get(0);
	CHECK(report.messageType == 2);
	CHECK(report.serviceType == 13);
	CHECK(report.readUint16() == 1);
	CHECK(report.readUint16() == 1);
	uint8_t string2[ECSS_MAX_FIXED_OCTET_STRING_SIZE];
	report.readOctetString(string2);
	auto a = String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>(string2, 8);
	CHECK(string.compare(a) == 0);
}

TEST_CASE("Last Downlink Part Report TM[13,3]", "[service][st13]") {
	String<ECSS_MAX_FIXED_OCTET_STRING_SIZE> string = String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>(
		"12345678");
	lPT.lastDownlinkPartReport(1, 1, string);
	REQUIRE(ServiceTests::hasOneMessage());
	Message report = ServiceTests::get(0);
	CHECK(report.messageType == 3);
	CHECK(report.serviceType == 13);
	CHECK(report.readUint16() == 1);
	CHECK(report.readUint16() == 1);
	uint8_t string2[ECSS_MAX_FIXED_OCTET_STRING_SIZE];
	report.readOctetString(string2);
	auto a = String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>(string2, 8);
	CHECK(string.compare(a) == 0);
}

TEST_CASE("First Uplink Part TC[13,9]", "[service][st13]") {
	String<256> string = "12345678";
	CHECK(string.compare(lPT.firstUplinkPart(string)) == 0);
}

TEST_CASE("Intermediate Uplink Part TC[13,10]", "[service][st13]") {
	String<256> string = "12345678";
	CHECK(string.compare(lPT.intermediateUplinkPart(string)) == 0);
}

TEST_CASE("Last Uplink Part TC[13,11]", "[service][st13]") {
	String<256> string = "12345678";
	CHECK(string.compare(lPT.lastUplinkPart(string)) == 0);
}
