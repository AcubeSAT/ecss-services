#include "Helpers/CRCHelper.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("CRC calculation - Basic String tests") {
	CHECK(CRCHelper::calculateCRC((uint8_t*) "Raccoon Squad!", 14) == 0x08FC);
	CHECK(CRCHelper::calculateCRC((uint8_t*) "ASAT", 4) == 0xBFFA);
	CHECK(CRCHelper::calculateCRC((uint8_t*) "All your space are belong to us", 31) == 0x545F);
	CHECK(CRCHelper::calculateCRC((uint8_t*) "SPAAAAAAAAACE!", 14) == 0xB441);
}

TEST_CASE("CRC calculation - Basic byte tests") {
	SECTION("ECSS compliance verification tests (p.617)") {
		uint8_t data1[2] = {0x00, 0x00};
		uint8_t data2[3] = {0x00, 0x00, 0x00};
		uint8_t data3[4] = {0xAB, 0xCD, 0xEF, 0x01};
		uint8_t data4[6] = {0x14, 0x56, 0xF8, 0x9A, 0x00, 0x01};

		CHECK(CRCHelper::calculateCRC(data1, 2) == 0x1D0F);
		CHECK(CRCHelper::calculateCRC(data2, 3) == 0xCC9C);
		CHECK(CRCHelper::calculateCRC(data3, 4) == 0x04A2);
		CHECK(CRCHelper::calculateCRC(data4, 6) == 0x7FD5);
	}

	SECTION("Null (0x00) before ending") {
		uint8_t data1[5] = {0x45, 0xF2, 0x00, 0xA2, 0x01};
		uint8_t data2[8] = {0x21, 0x65, 0xDF, 0x00, 0xC4, 0x00, 0x00, 0xBF};
		uint8_t data3[4] = {0x07, 0x00, 0x05, 0xFF};

		CHECK(CRCHelper::calculateCRC(data1, 5) == 0x3A2B);
		CHECK(CRCHelper::calculateCRC(data2, 8) == 0x89EE);
		CHECK(CRCHelper::calculateCRC(data3, 4) == 0x34E8);
	}
}

TEST_CASE("CRC validation - Basic tests") {
	uint8_t data1[7] = {'H', 'e', 'l', 'l', 'o', 0xDA, 0xDA};
	uint8_t data2[6] = {'A', 'S', 'A', 'T', 0xBF, 0xFA};
	uint8_t data3[10] = {'S', 'p', 'A', '@', 'A', 'c', '3', '!', 0xB4, 0x41};
	// checksum from original SPAAAAAAAAACE! string
	uint8_t data4[6] = {'A', 0x43, 0x52, 0xDF, 0xBF, 0xFA};
	// ASAT, but "corrupted" with the last 2 bytes the original checksum of the 'ASAT' string

	uint8_t data5[9] = {'C', 'U', 'B', 'E', 'S', 'A', 'T', 0x53, 0x15}; // corrupted CRC checksum

	CHECK(CRCHelper::validateCRC(data1, 7) == 0x0);
	CHECK(CRCHelper::validateCRC(data2, 6) == 0x0);
	CHECK(CRCHelper::validateCRC(data3, 10) != 0x0);
	CHECK(CRCHelper::validateCRC(data4, 6) != 0x0);
	CHECK(CRCHelper::validateCRC(data5, 9) != 0x0);
}
