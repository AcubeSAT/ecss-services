bool is_leap_year(uint16_t year);

////////////: CONSTRUCTOR ////////////
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(){
  tai_counter = 0;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(int seconds){
  tai_counter = (tai_counter_t)seconds << 8*fractional_counter_bytes;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(Acubesat_CDS_timestamp timestamp){
  tai_counter = 0; //TODO
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(etl::array<uint8_t, 9> timestamp){
  //process header
  int header_size = bool(timestamp[0] >> 7) ? 2 : 1;
  int timestamp_fractional_bytes = 0;
  int timestamp_seconds_bytes = 1;
  int epoch_param = 0;

  if (header_size == 2){
    epoch_param = (timestamp[0] & 0b01110000) >> 4;
    timestamp_seconds_bytes += ((timestamp[0] & 0b00001100) >> 2) + (timestamp[1] & 0b01100000) >> 5;
    timestamp_fractional_bytes = ((timestamp[0] & 0b00000011) >> 0) + ((timestamp[1] & 0b00011000) >> 3);
  }
  else if(header_size==1){
    epoch_param = (timestamp[0] & 0b01110000) >> 4;
    timestamp_seconds_bytes += (timestamp[0] & 0b00001100) >> 2;
    timestamp_fractional_bytes = (timestamp[0] & 0b00000011) >> 0;
  }
  else{
    ASSERT_INTERNAL(true, ErrorHandler::InternalErrorType::InvalidDate);
  }

  //check input validity (useless bytes set to 0)
  int err = 0;
  for (int i=header_size+timestamp_seconds_bytes+timestamp_fractional_bytes; i<9; i++){
    if (timestamp[i] != 0) {
      err += 1;
    }
  }
  ASSERT_INTERNAL(err == 0, ErrorHandler::InternalErrorType::InvalidDate);

  //do checks wrt template precision parameters
  ASSERT_INTERNAL(timestamp_seconds_bytes <= seconds_counter_bytes, ErrorHandler::InternalErrorType::InvalidDate);
  ASSERT_INTERNAL(timestamp_fractional_bytes <= fractional_counter_bytes, ErrorHandler::InternalErrorType::InvalidDate);

  //put timestamp into internal counter
  tai_counter = 0;
  //add seconds until run out of bytes on input array
  for(auto i = 0; i < timestamp_seconds_bytes; i++){
    tai_counter = tai_counter << 8;
    tai_counter += timestamp[header_size + i];
  }
  //add fractional until run out of bytes on input array
  for(auto i = 0; i < timestamp_fractional_bytes; i++){
    tai_counter = tai_counter << 8;
    tai_counter += timestamp[header_size + timestamp_seconds_bytes + i];
  }
  //pad rightmost bytes to full length
  tai_counter = tai_counter << 8*(fractional_counter_bytes - timestamp_fractional_bytes);
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(UTC_Timestamp timestamp){
  int secs = UNIX_TO_ACUBESAT_EPOCH_ELAPSED_SECONDS;
  for (int y = ACUBESAT_EPOCH_YEAR; y < timestamp.year; ++y) {
    secs += (is_leap_year(y) ? 366 : 365) * SECONDS_PER_DAY;
  }
  for (int m = ACUBESAT_EPOCH_MONTH; m < timestamp.month; ++m) {
    secs += DaysOfMonth[m - 1U] * SECONDS_PER_DAY;
    if ((m == 2U) && is_leap_year(timestamp.year)) {
      secs += SECONDS_PER_DAY;
    }
  }
  secs += (timestamp.day - ACUBESAT_EPOCH_DAY) * SECONDS_PER_DAY;
  secs += timestamp.hour * SECONDS_PER_HOUR;
  secs += timestamp.minute * SECONDS_PER_MINUTE;
  secs += timestamp.second;
  this.Instant(secs);
}

////////////// GETTER ///////////////
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const int Instant<seconds_counter_bytes, fractional_counter_bytes>::as_TAI_seconds(){
  return tai_counter >> (8*fractional_counter_bytes);
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
// TODO


////////////// TESTS ///////////////
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const std::type_info& Instant<seconds_counter_bytes, fractional_counter_bytes>::check_header_type(){
  return typeid(CUC_header);
}
