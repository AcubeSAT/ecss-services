#include "Helpers/PacketStore.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("Counting a packet store's size in bytes") {
	SECTION("Correct counting of size in bytes") {
		Message tm1;
		tm1.appendUint8(4);
		tm1.appendFloat(5.6);

		PacketStore packetStore;
		packetStore.storedTelemetryPackets.push_back({2, tm1});

		REQUIRE(packetStore.storedTelemetryPackets.size() == 1);
		REQUIRE(packetStore.calculateSizeInBytes() == 5);

		Message tm2;
		tm2.appendBoolean(true);
		tm2.appendUint16(45);
		tm2.appendUint8(3);
		tm2.appendUint32(55);

		packetStore.storedTelemetryPackets.push_back({2, tm2});

		REQUIRE(packetStore.storedTelemetryPackets.size() == 2);
		REQUIRE(packetStore.calculateSizeInBytes() == 13);

		Message tm3;
		tm3.appendUint64(743);
		tm3.appendUint8(3);
		tm3.appendUint32(55);

		packetStore.storedTelemetryPackets.push_back({3, tm3});

		REQUIRE(packetStore.storedTelemetryPackets.size() == 3);
		REQUIRE(packetStore.calculateSizeInBytes() == 26);
	}
}