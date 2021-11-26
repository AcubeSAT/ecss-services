#define SECONDS_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1 0b00001100
#define SECONDS_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE2 0b01100000
#define FRACTIONAL_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1 0b00000011
#define FRACTIONAL_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE2 0b00011000

#define SECONDS_FROM_SINGLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1 0b00001100
#define FRACTIONAL_FROM_SINGLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1 0b00000011

bool is_leap_year(uint16_t year);

////////////: CONSTRUCTORS ////////////
//// FROM CDS TIMESTAMP
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(AcubeSAT_CDS_timestamp timestamp){
  //tai_counter = 0; //See Issue #105 on Gitlab
}

//// FROM CUC TIMESTAMP
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(etl::array<uint8_t, MAXIMUM_BYTES_FOR_COMPLETE_CUC_TIMESTAMP> timestamp){
  //process header
  int header_size = 1;
  if(timestamp[0] & 0b10000000){
    header_size = 2;
  };

  int timestamp_fractional_bytes_count = 0;
  int timestamp_seconds_bytes_count = 1;
  //int epoch_param = 0;

  if (header_size == 2){
    //epoch_param = (timestamp[0] & 0b01110000) >> 4;
    timestamp_seconds_bytes_count += (timestamp[0] & SECONDS_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1) >> 2;
    timestamp_seconds_bytes_count += (timestamp[1] & SECONDS_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE2) >> 5;
    timestamp_fractional_bytes_count = ((timestamp[0] & FRACTIONAL_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1) >> 0) + ((timestamp[1] & FRACTIONAL_FROM_DOUBLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE2) >> 3);
  }
  else if(header_size==1){
    //epoch_param = (timestamp[0] & 0b01110000) >> 4;
    timestamp_seconds_bytes_count += (timestamp[0] & SECONDS_FROM_SINGLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1) >> 2;
    timestamp_fractional_bytes_count = (timestamp[0] & FRACTIONAL_FROM_SINGLE_BYTE_CUC_TIMESTAMP_BITMASK_BYTE1) >> 0;
  }
  else{
    ASSERT_INTERNAL(true, ErrorHandler::InternalErrorType::InvalidDate);
  }

  //check input validity (useless bytes set to 0)
  int err = 0;
  for (int i=header_size+timestamp_seconds_bytes_count+timestamp_fractional_bytes_count; i<9; i++){
    if (timestamp[i] != 0) {
      err += 1;
      break;
    }
  }
  ASSERT_INTERNAL(err == 0, ErrorHandler::InternalErrorType::InvalidDate);

  //do checks wrt template precision parameters
  ASSERT_INTERNAL(timestamp_seconds_bytes_count <= seconds_counter_bytes, ErrorHandler::InternalErrorType::InvalidDate);
  ASSERT_INTERNAL(timestamp_fractional_bytes_count <= fractional_counter_bytes, ErrorHandler::InternalErrorType::InvalidDate);

  //put timestamp into internal counter
  tai_counter = 0;
  //add seconds until run out of bytes on input array
  for(auto i = 0; i < timestamp_seconds_bytes_count; i++){
    tai_counter = tai_counter << 8;
    tai_counter += timestamp[header_size + i];
  }
  //add fractional until run out of bytes on input array
  for(auto i = 0; i < timestamp_fractional_bytes_count; i++){
    tai_counter = tai_counter << 8;
    tai_counter += timestamp[header_size + timestamp_seconds_bytes_count + i];
  }
  //pad rightmost bytes to full length
  tai_counter = tai_counter << 8*(fractional_counter_bytes - timestamp_fractional_bytes_count);
}

//// FROM UTC TIMESTAMP
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(UTC_Timestamp timestamp){
  int seconds = UNIX_TO_ACUBESAT_EPOCH_ELAPSED_SECONDS;
  for (int year = ACUBESAT_EPOCH_YEAR; year < timestamp.year; ++year) {
    seconds += (is_leap_year(year) ? 366 : 365) * SECONDS_PER_DAY;
  }
  for (int month = ACUBESAT_EPOCH_MONTH; month < timestamp.month; ++month) {
    seconds += DAYSOFMONTH[month - 1] * SECONDS_PER_DAY;
    if ((month == 2U) && is_leap_year(timestamp.year)) {
      seconds += SECONDS_PER_DAY;
    }
  }
  seconds += (timestamp.day - ACUBESAT_EPOCH_DAY) * SECONDS_PER_DAY;
  seconds += timestamp.hour * SECONDS_PER_HOUR;
  seconds += timestamp.minute * SECONDS_PER_MINUTE;
  seconds += timestamp.second;
  tai_counter = static_cast<tai_counter_t>(seconds) << 8*fractional_counter_bytes;
}

////////////// GETTER ///////////////
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const int Instant<seconds_counter_bytes, fractional_counter_bytes>::as_TAI_seconds(){
  return tai_counter >> (8*fractional_counter_bytes);
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const etl::array<uint8_t, MAXIMUM_BYTES_FOR_COMPLETE_CUC_TIMESTAMP> Instant<seconds_counter_bytes, fractional_counter_bytes>::as_CUC_timestamp(){
  etl::array<uint8_t, MAXIMUM_BYTES_FOR_COMPLETE_CUC_TIMESTAMP> return_array = {0};
  int index_first_non_header_byte;

  static constexpr uint8_t header_size = seconds_counter_bytes < 4 && fractional_counter_bytes < 3 ? 1 : 2; //number of bytes in CUC header

  if (header_size == 1){
    return_array[0] = static_cast<uint8_t>(CUC_header);
    index_first_non_header_byte = 1;
  }

  else{ //two-bytes CUC header
    return_array[1] = static_cast<uint8_t>(CUC_header);
    return_array[0] = static_cast<uint8_t>(CUC_header >> 8);
    index_first_non_header_byte = 2;
  }

  for(auto byte_being_filled_in_return = 0; byte_being_filled_in_return < seconds_counter_bytes + fractional_counter_bytes; byte_being_filled_in_return++){
    int index_of_interesting_byte_in_tai_counter = 8*(seconds_counter_bytes + fractional_counter_bytes - byte_being_filled_in_return - 1);
    return_array[index_first_non_header_byte + byte_being_filled_in_return] = tai_counter >> index_of_interesting_byte_in_tai_counter;
  }

  return return_array;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const UTC_Timestamp Instant<seconds_counter_bytes, fractional_counter_bytes>::as_UTC_timestamp(){
  int seconds = as_TAI_seconds();

  // elapsed seconds should be between dates, that are after 1/1/2019 and Unix epoch
	ASSERT_INTERNAL(seconds >= UNIX_TO_ACUBESAT_EPOCH_ELAPSED_SECONDS, ErrorHandler::InternalErrorType::InvalidDate);

	seconds -= UNIX_TO_ACUBESAT_EPOCH_ELAPSED_SECONDS; // elapsed seconds from Unix epoch until 1/1/2019 00:00:00 (UTC)
	int year = 2019;
	int month = 1;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;

	// calculate years
	while (seconds >= (is_leap_year(year) ? 366 : 365) * SECONDS_PER_DAY) {
		seconds -= (is_leap_year(year) ? 366 : 365) * SECONDS_PER_DAY;
		year++;
	}

	// calculate months
	int i = 0;
	while (seconds >= (DAYSOFMONTH[i] * SECONDS_PER_DAY)) {
		month++;
		seconds -= (DAYSOFMONTH[i] * SECONDS_PER_DAY);
		i++;
		if ((i == 1U) && is_leap_year(year)) {
			if (seconds <= (28 * SECONDS_PER_DAY)) {
				break;
			}
			month++;
			seconds -= 29 * SECONDS_PER_DAY;
			i++;
		}
	}

	// calculate days
	day = seconds / SECONDS_PER_DAY;
	seconds -= day * SECONDS_PER_DAY;
	day++; // add 1 day because we start count from 1 January (and not 0 January!)

	// calculate hours
	hour = seconds / SECONDS_PER_HOUR;
	seconds -= hour * SECONDS_PER_HOUR;

	// calculate minutes
	minute = seconds / SECONDS_PER_MINUTE;
	seconds -= minute * SECONDS_PER_MINUTE;

	// calculate seconds
	second = seconds;

  return UTC_Timestamp(year, month, day, hour, minute, second);
}

////////////// OPERATORS ///////////
/// COMPARISON
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool Instant<seconds_counter_bytes, fractional_counter_bytes>::operator==(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const{
  return tai_counter == Instant.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool Instant<seconds_counter_bytes, fractional_counter_bytes>::operator!=(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const{
  return tai_counter != Instant.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool Instant<seconds_counter_bytes, fractional_counter_bytes>::operator<(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const{
  return tai_counter < Instant.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool Instant<seconds_counter_bytes, fractional_counter_bytes>::operator>(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const{
  return tai_counter > Instant.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool Instant<seconds_counter_bytes, fractional_counter_bytes>::operator<=(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const{
  return tai_counter <= Instant.tai_counter;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
bool Instant<seconds_counter_bytes, fractional_counter_bytes>::operator>=(const Instant<seconds_counter_bytes, fractional_counter_bytes>& Instant) const{
  return tai_counter >= Instant.tai_counter;
}
/// ARITHMETIC
// See Issue #104 on Gitlab repository
