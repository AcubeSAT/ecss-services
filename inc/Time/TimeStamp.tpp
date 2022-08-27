#include <cmath>
#include "TimeStamp.hpp"

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
constexpr bool TimeStamp<BaseBytes, FractionBytes, Num, Denom>::areSecondsValid(TimeStamp::TAICounter_t seconds) {
	// TODO: See what happens with num/denom
	return seconds < MaxSeconds;
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
TimeStamp<BaseBytes, FractionBytes, Num, Denom>::TimeStamp(uint64_t taiSecondsFromEpoch) {
	ASSERT_INTERNAL(areSecondsValid((taiSecondsFromEpoch)), ErrorHandler::InternalErrorType::InvalidTimeStampInput);

	using FromDuration = std::chrono::duration<uint64_t>;
	const auto duration = FromDuration(taiSecondsFromEpoch);

	taiCounter = std::chrono::duration_cast<RawDuration>(duration).count();
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
TimeStamp<BaseBytes, FractionBytes, Num, Denom>::TimeStamp(Time::CustomCUC_t customCUCTimestamp) {
	//TODO Remove CustomCUC_t class
	TimeStamp<8, 0, 1, 10> input;
	input.taiCounter = customCUCTimestamp.elapsed100msTicks;

	new (this) TimeStamp<BaseBytes, FractionBytes, Num, Denom>(input);
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
TimeStamp<BaseBytes, FractionBytes, Num, Denom>::TimeStamp(etl::array<uint8_t, Time::CUCTimestampMaximumSize> timestamp) {
	// process header
	uint8_t headerSize = 1;
	if ((timestamp[0] & 0b10000000U) != 0) {
		headerSize = 2;
	}

	uint8_t inputBaseBytes = ((timestamp[0] & 0b00001100U) >> 2U) + 1U;
	uint8_t inputFractionBytes = (timestamp[0] & 0b00000011U) >> 0U;

	if (headerSize == 2) {
		inputBaseBytes += (timestamp[1] & 0b01100000U) >> 5U;
		inputFractionBytes += (timestamp[1] & 0b00011100U) >> 2U;
	}

	// check input validity (useless bytes set to 0)
	for (int i = headerSize + inputBaseBytes + inputFractionBytes; i < Time::CUCTimestampMaximumSize; i++) {
		if (timestamp[i] != 0) {
			ErrorHandler::reportInternalError(ErrorHandler::InternalErrorType::InvalidTimeStampInput);
			break;
		}
	}

	// do checks wrt template precision parameters
	ASSERT_INTERNAL(inputBaseBytes <= BaseBytes, ErrorHandler::InternalErrorType::InvalidTimeStampInput);
	ASSERT_INTERNAL(inputFractionBytes <= FractionBytes, ErrorHandler::InternalErrorType::InvalidTimeStampInput);

	// put timestamp into internal counter
	taiCounter = 0;
	// add seconds until run out of bytes on input array
	for (auto i = 0; i < inputBaseBytes + inputFractionBytes; i++) {
		taiCounter = taiCounter << 8;
		taiCounter += timestamp[headerSize + i];
	}
	// pad rightmost bytes to full length
	taiCounter = taiCounter << 8 * (FractionBytes - inputFractionBytes);
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
TimeStamp<BaseBytes, FractionBytes, Num, Denom>::TimeStamp(const UTCTimestamp& timestamp) {
	TAICounter_t seconds = 0;

	/**
	 * Add to the seconds variable, with an overflow check
	 */
	auto secondsAdd = [&seconds](TAICounter_t value) {
		seconds += value;
		if (seconds < value) {
			ErrorHandler::reportInternalError(ErrorHandler::TimeStampOutOfBounds);
		}
	};

	for (int year = Time::Epoch.year; year < timestamp.year; ++year) {
		secondsAdd((Time::isLeapYear(year) ? 366 : 365) * Time::SecondsPerDay);
	}
	for (int month = Time::Epoch.month; month < timestamp.month; ++month) {
		secondsAdd(Time::DaysOfMonth[month - 1] * Time::SecondsPerDay);
		if ((month == 2U) && Time::isLeapYear(timestamp.year)) {
			secondsAdd(Time::SecondsPerDay);
		}
	}

	secondsAdd((timestamp.day - Time::Epoch.day) * Time::SecondsPerDay);
	secondsAdd(timestamp.hour * Time::SecondsPerHour);
	secondsAdd(timestamp.minute * Time::SecondsPerMinute);
	secondsAdd(timestamp.second);

	ASSERT_INTERNAL(areSecondsValid(seconds), ErrorHandler::TimeStampOutOfBounds);

	taiCounter = static_cast<TAICounter_t>(seconds) << (8 * FractionBytes);
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
typename TimeStamp<BaseBytes, FractionBytes, Num, Denom>::TAICounter_t
TimeStamp<BaseBytes, FractionBytes, Num, Denom>::asTAIseconds() {
	const auto duration = RawDuration(taiCounter);
	using ToDuration = std::chrono::duration<TAICounter_t>;

	return std::chrono::duration_cast<ToDuration>(duration).count();
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
Time::CustomCUC_t TimeStamp<BaseBytes, FractionBytes, Num, Denom>::asCustomCUCTimestamp() {
	//TODO: Remove CustomCUC_t class
	TimeStamp<8, 0, 1, 10> converted(*this);
	return {converted.taiCounter};
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
template <typename T>
T TimeStamp<BaseBytes, FractionBytes, Num, Denom>::asTAIseconds() {
	static_assert(std::is_floating_point_v<T>, "TimeStamp::asTAIseconds() only accepts numeric types.");
	static_assert(std::numeric_limits<T>::max() >= MaxSeconds);

	TAICounter_t decimalPart = taiCounter >> (8 * FractionBytes);

	T fractionalPart = taiCounter - (decimalPart << (8 * FractionBytes));
	T fractionalPartMax = (1U << (8U * FractionBytes)) - 1U;

	return decimalPart + fractionalPart / fractionalPartMax;
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
etl::array<uint8_t, Time::CUCTimestampMaximumSize> TimeStamp<BaseBytes, FractionBytes, Num, Denom>::toCUCtimestamp() {
	etl::array<uint8_t, Time::CUCTimestampMaximumSize> returnArray = {0};

	static constexpr uint8_t headerBytes = (BaseBytes < 4 && FractionBytes < 3) ? 1 : 2;

	if (headerBytes == 1) {
		returnArray[0] = static_cast<uint8_t>(CUCHeader);
	} else {
		returnArray[1] = static_cast<uint8_t>(CUCHeader);
		returnArray[0] = static_cast<uint8_t>(CUCHeader >> 8);
	}

	for (auto byte = 0; byte < BaseBytes + FractionBytes; byte++) {
		uint8_t taiCounterIndex = 8 * (BaseBytes + FractionBytes - byte - 1);
		returnArray[headerBytes + byte] = taiCounter >> taiCounterIndex;
	}

	return returnArray;
}

template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
UTCTimestamp TimeStamp<BaseBytes, FractionBytes, Num, Denom>::toUTCtimestamp() {
	using namespace Time;

	uint32_t totalSeconds = asTAIseconds();

	uint16_t yearUTC = Epoch.year;
	uint8_t monthUTC = Epoch.month;
	uint8_t dayUTC = Epoch.day;
	uint8_t hour = 0;
	uint8_t minute = 0;
	uint8_t second = 0;

	// calculate years
	while (totalSeconds >= (isLeapYear(yearUTC) ? 366 : 365) * SecondsPerDay) {
		totalSeconds -= (isLeapYear(yearUTC) ? 366 : 365) * SecondsPerDay;
		yearUTC++;
	}

	// calculate months
	int currentMonth = 0;
	while (totalSeconds >= (DaysOfMonth[currentMonth] * SecondsPerDay)) {
		monthUTC++;
		totalSeconds -= (DaysOfMonth[currentMonth] * SecondsPerDay);
		currentMonth++;
		if ((currentMonth == 1U) && isLeapYear(yearUTC)) {
			if (totalSeconds <= (28 * SecondsPerDay)) {
				break;
			}
			monthUTC++;
			totalSeconds -= 29 * SecondsPerDay;
			currentMonth++;
		}
	}

	dayUTC = totalSeconds / SecondsPerDay;
	totalSeconds -= dayUTC * SecondsPerDay;
	dayUTC++; // add 1 day because we start count from 1 January (and not 0 January!)

	hour = totalSeconds / SecondsPerHour;
	totalSeconds -= hour * SecondsPerHour;

	minute = totalSeconds / SecondsPerMinute;
	totalSeconds -= minute * SecondsPerMinute;

	second = totalSeconds;

	return {yearUTC, monthUTC, dayUTC, hour, minute, second};
}
template <uint8_t BaseBytes, uint8_t FractionBytes, int Num, int Denom>
template <uint8_t BaseBytesIn, uint8_t FractionBytesIn, int NumIn, int DenomIn>
TimeStamp<BaseBytes, FractionBytes, Num, Denom>::TimeStamp(TimeStamp<BaseBytesIn, FractionBytesIn, NumIn, DenomIn> input) {
	if constexpr (std::is_same_v<decltype(*this), decltype(input)>) {
		taiCounter = input.taiCounter;
		return;
	}

	constexpr double InputRatio = static_cast<double>(NumIn) / DenomIn;
	constexpr double OutputRatio = static_cast<double>(Num) / Denom;

	double inputSeconds = input.taiCounter / static_cast<double>(1 << (8 * FractionBytesIn));
	inputSeconds *= InputRatio;

	double outputSeconds = inputSeconds / OutputRatio * (1UL << (8 * FractionBytes));
	if (outputSeconds > MaxSeconds) {
		ErrorHandler::reportInternalError(ErrorHandler::TimeStampOutOfBounds);
	}

	taiCounter = static_cast<TAICounter_t>(round(outputSeconds));
}
