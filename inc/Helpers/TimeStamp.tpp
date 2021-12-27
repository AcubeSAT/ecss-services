#define SECONDS_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1 0b00001100
#define SECONDS_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE2 0b01100000
#define FRACTIONAL_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1 0b00000011
#define FRACTIONAL_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE2 0b00011000

#define SECONDS_FROM_SINGLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1 0b00001100
#define FRACTIONAL_FROM_SINGLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1 0b00000011

////////////: CONSTRUCTORS ////////////

//// FROM CUC TIMESTAMP
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::TimeStamp(
    etl::array<uint8_t, Time::CUCTimestampMaximumSize> timestamp) {
	// process header
	int header_size = 1;
	if (timestamp[0] & 0b10000000) {
		header_size = 2;
	};

	int timestamp_fractional_bytes_count = 0;
	int timestamp_seconds_bytes_count = 1;
	// int epoch_param = 0;

	if (header_size == 2) {
		// epoch_param = (timestamp[0] & 0b01110000) >> 4;
		timestamp_seconds_bytes_count += (timestamp[0] & SECONDS_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1) >> 2;
		timestamp_seconds_bytes_count += (timestamp[1] & SECONDS_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE2) >> 5;
		timestamp_fractional_bytes_count =
		    ((timestamp[0] & FRACTIONAL_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1) >> 0) +
		    ((timestamp[1] & FRACTIONAL_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE2) >> 3);
	} else if (header_size == 1) {
		// epoch_param = (timestamp[0] & 0b01110000) >> 4;
		timestamp_seconds_bytes_count += (timestamp[0] & SECONDS_FROM_SINGLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1) >> 2;
		timestamp_fractional_bytes_count =
		    (timestamp[0] & FRACTIONAL_FROM_SINGLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1) >> 0;
	} else {
		ASSERT_INTERNAL(true, ErrorHandler::InternalErrorType::InvalidTimeStampInput);
	}

	// check input validity (useless bytes set to 0)
	int err = 0;
	for (int i = header_size + timestamp_seconds_bytes_count + timestamp_fractional_bytes_count; i < 9; i++) {
		if (timestamp[i] != 0) {
			err += 1;
			break;
		}
	}
	ASSERT_INTERNAL(err == 0, ErrorHandler::InternalErrorType::InvalidTimeStampInput);

	// do checks wrt template precision parameters
	ASSERT_INTERNAL(timestamp_seconds_bytes_count <= seconds_counter_bytes,
	                ErrorHandler::InternalErrorType::InvalidTimeStampInput);
	ASSERT_INTERNAL(timestamp_fractional_bytes_count <= fractional_counter_bytes,
	                ErrorHandler::InternalErrorType::InvalidTimeStampInput);

	// put timestamp into internal counter
	tai_counter = 0;
	// add seconds until run out of bytes on input array
	for (auto i = 0; i < timestamp_seconds_bytes_count; i++) {
		tai_counter = tai_counter << 8;
		tai_counter += timestamp[header_size + i];
	}
	// add fractional until run out of bytes on input array
	for (auto i = 0; i < timestamp_fractional_bytes_count; i++) {
		tai_counter = tai_counter << 8;
		tai_counter += timestamp[header_size + timestamp_seconds_bytes_count + i];
	}
	// pad rightmost bytes to full length
	tai_counter = tai_counter << 8 * (fractional_counter_bytes - timestamp_fractional_bytes_count);
}

//// FROM UTC TIMESTAMP
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::TimeStamp(UTCTimestamp timestamp) {
	int seconds = Time::EpochSecondsFromUnix;
	for (int year = Time::Epoch.year; year < timestamp.year; ++year) {
		seconds += (Time::isLeapYear(year) ? 366 : 365) * Time::SecondsPerDay;
	}
	for (int month = Time::Epoch.month; month < timestamp.month; ++month) {
		seconds += Time::DaysOfMonth[month - 1] * Time::SecondsPerDay;
		if ((month == 2U) && Time::isLeapYear(timestamp.year)) {
			seconds += Time::SecondsPerDay;
		}
	}
	seconds += (timestamp.day - Time::Epoch.day) * Time::SecondsPerDay;
	seconds += timestamp.hour * Time::SecondsPerHour;
	seconds += timestamp.minute * Time::SecondsPerMinute;
	seconds += timestamp.second;
	tai_counter = static_cast<tai_counter_t>(seconds) << 8 * fractional_counter_bytes;
}

////////////// GETTER ///////////////
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const int TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::as_TAI_seconds() {
	return tai_counter >> (8 * fractional_counter_bytes);
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const etl::array<uint8_t, Time::CUCTimestampMaximumSize>
TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::as_CUC_timestamp() {
	etl::array<uint8_t, Time::CUCTimestampMaximumSize> return_array = {0};
	int index_first_non_header_byte;

  // cppcheck-suppress redundantCondition
	static constexpr uint8_t header_size = seconds_counter_bytes < 4 && fractional_counter_bytes < 3 ? 1 : 2; // number of bytes in CUC header

	if (header_size == 1) {
		return_array[0] = static_cast<uint8_t>(CUC_header);
		index_first_non_header_byte = 1;
	}

	else { // two-bytes CUC header
		return_array[1] = static_cast<uint8_t>(CUC_header);
		return_array[0] = static_cast<uint8_t>(CUC_header >> 8);
		index_first_non_header_byte = 2;
	}

	for (auto byte_being_filled_in_return = 0;
	     byte_being_filled_in_return < seconds_counter_bytes + fractional_counter_bytes;
	     byte_being_filled_in_return++) {
		int index_of_interesting_byte_in_tai_counter =
		    8 * (seconds_counter_bytes + fractional_counter_bytes - byte_being_filled_in_return - 1);
		return_array[index_first_non_header_byte + byte_being_filled_in_return] =
		    tai_counter >> index_of_interesting_byte_in_tai_counter;
	}

	return return_array;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const UTCTimestamp TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::as_UTC_timestamp() {
	using namespace Time;

	int seconds = as_TAI_seconds();

	// elapsed seconds should be between dates, that are after 1/1/2019 and Unix epoch
	ASSERT_INTERNAL(seconds >= EpochSecondsFromUnix,
	                ErrorHandler::InternalErrorType::InvalidDate);

	seconds -= EpochSecondsFromUnix; // elapsed seconds from Unix epoch until AcubeSAT custom
	// epoch 00:00:00 (UTC)
	int year_utc = Epoch.year;
	int month_utc = Epoch.month;
	int day_utc = Epoch.day;
	int hour = 0;
	int minute = 0;
	int second = 0;

	// calculate years
	while (seconds >= (isLeapYear(year_utc) ? 366 : 365) * SecondsPerDay) {
		seconds -= (isLeapYear(year_utc) ? 366 : 365) * SecondsPerDay;
		year_utc++;
	}

	// calculate months
	int current_month = 0;
	while (seconds >= (DaysOfMonth[current_month] * SecondsPerDay)) {
		month_utc++;
		seconds -= (DaysOfMonth[current_month] * SecondsPerDay);
		current_month++;
		if ((current_month == 1U) && isLeapYear(year_utc)) {
			if (seconds <= (28 * SecondsPerDay)) {
				break;
			}
			month_utc++;
			seconds -= 29 * SecondsPerDay;
			current_month++;
		}
	}

	// calculate days
	day_utc = seconds / SecondsPerDay;
	seconds -= day_utc * SecondsPerDay;
	day_utc++; // add 1 day because we start count from 1 January (and not 0 January!)

	// calculate hours
	hour = seconds / SecondsPerHour;
	seconds -= hour * SecondsPerHour;

	// calculate minutes
	minute = seconds / SecondsPerMinute;
	seconds -= minute * SecondsPerMinute;

	// calculate seconds
	second = seconds;

	return UTCTimestamp(year_utc, month_utc, day_utc, hour, minute, second);
}

////////////// OPERATORS ///////////
/// COMPARISON
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::operator==(
    const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const {
	return tai_counter == TimeStamp.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::operator!=(
    const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const {
	return tai_counter != TimeStamp.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::operator<(
    const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const {
	return tai_counter < TimeStamp.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::operator>(
    const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const {
	return tai_counter > TimeStamp.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::operator<=(
    const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const {
	return tai_counter <= TimeStamp.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool TimeStamp<seconds_counter_bytes, fractional_counter_bytes>::operator>=(
    const TimeStamp<seconds_counter_bytes, fractional_counter_bytes>& TimeStamp) const {
	return tai_counter >= TimeStamp.tai_counter;
}
/// ARITHMETIC
// See Issue #104 on Gitlab repository
