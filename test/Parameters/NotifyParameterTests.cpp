#include "Helpers/NotifyParameter.hpp"
#include "Message.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("Notify Parameter: Notifier") {
	int counter = 0;

	NotifyParameter<uint32_t> parameter(0);

	SECTION("Notifier not set") {
		parameter.setValueLoudly(1);
		CHECK(counter == 0);
	}

	parameter.setNotifier([&counter](auto) -> auto{
		counter++;
	});
	CHECK(counter == 1);

	parameter.setValueLoudly(2);
	CHECK(counter == 2);

	parameter.setValueLoudly(2);
	CHECK(counter == 3);

	parameter.unsetNotifier();
	parameter.setValueLoudly(3);
	CHECK(counter == 3);
}

TEST_CASE("Notify Parameter: Messages") {
	int16_t storage;

	NotifyParameter<uint32_t> parameter(
	    0, [&storage](auto v) -> auto{
		    storage = v;
	    });

	Message message(0, 0, Message::TC);
	message.appendUint32(184);

	parameter.setValueFromMessage(message);

	CHECK(storage == 184);
}

TEST_CASE("Notify Parameter: Extra functionality") {
	int counter = 0;

	NotifyParameter<uint32_t> parameter(0);
	auto notifier = [&counter](auto) -> auto{
		counter++;
	};

	parameter.setNotifier(notifier);
	CHECK(counter == 1);

	parameter.setNotifier(notifier, false);
	CHECK(counter == 1);

	parameter.notify();
	CHECK(counter == 2);
}