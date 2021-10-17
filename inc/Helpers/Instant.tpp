bool is_leap_year(uint16_t year);

////////////: CONSTRUCTORS ////////////
//// FROM CDS TIMESTAMP
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(Acubesat_CDS_timestamp timestamp){
  //tai_counter = 0; //TODO
}

//// FROM CUC TIMESTAMP
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(etl::array<uint8_t, 9> timestamp){
  //process header
  int header_size = bool(timestamp[0] >> 7) ? 2 : 1;
  int timestamp_fractional_bytes = 0;
  int timestamp_seconds_bytes = 1;
  //int epoch_param = 0;

  if (header_size == 2){
    //epoch_param = (timestamp[0] & 0b01110000) >> 4;
    timestamp_seconds_bytes += (timestamp[0] & 0b00001100) >> 2;
    timestamp_seconds_bytes += (timestamp[1] & 0b01100000) >> 5;
    timestamp_fractional_bytes = ((timestamp[0] & 0b00000011) >> 0) + ((timestamp[1] & 0b00011000) >> 3);
  }
  else if(header_size==1){
    //epoch_param = (timestamp[0] & 0b01110000) >> 4;
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
  //printf("created timestamp from %d seconds bytes, %d fractional bytes, leading to an internal value of %lu\n", timestamp_seconds_bytes, timestamp_fractional_bytes, tai_counter);
}

//// FROM UTC TIMESTAMP
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(UTC_Timestamp timestamp){
  int secs = UNIX_TO_ACUBESAT_EPOCH_ELAPSED_SECONDS;
  for (int y = ACUBESAT_EPOCH_YEAR; y < timestamp.year; ++y) {
    secs += (is_leap_year(y) ? 366 : 365) * SECONDS_PER_DAY;
  }
  for (int m = ACUBESAT_EPOCH_MONTH; m < timestamp.month; ++m) {
    secs += DaysOfMonth[m - 1] * SECONDS_PER_DAY;
    if ((m == 2U) && is_leap_year(timestamp.year)) {
      secs += SECONDS_PER_DAY;
    }
  }
  secs += (timestamp.day - ACUBESAT_EPOCH_DAY) * SECONDS_PER_DAY;
  secs += timestamp.hour * SECONDS_PER_HOUR;
  secs += timestamp.minute * SECONDS_PER_MINUTE;
  secs += timestamp.second;
  tai_counter = static_cast<tai_counter_t>(secs) << 8*fractional_counter_bytes;
}

////////////// GETTER ///////////////
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const int Instant<seconds_counter_bytes, fractional_counter_bytes>::as_TAI_seconds(){
  return tai_counter >> (8*fractional_counter_bytes);
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const etl::array<uint8_t, 9> Instant<seconds_counter_bytes, fractional_counter_bytes>::as_CUC_timestamp(){
  etl::array<uint8_t, 9> r = {0};
  int i0;
  if (typeid(CUC_header_t).name() == typeid(uint8_t).name()){ //one-byte CUC header
    r[0] = static_cast<uint8_t>(CUC_header);
    i0 = 1;
  }

  else{ //two-bytes CUC header
    r[1] = static_cast<uint8_t>(CUC_header);
    r[0] = static_cast<uint8_t>(CUC_header >> 8);
    i0 = 2;
  }

  //printf("tai_counter of size %lu\n", sizeof(tai_counter_t));
  //printf("total bytes size %d\n", seconds_counter_bytes + fractional_counter_bytes);
  for(auto i = 0; i < seconds_counter_bytes + fractional_counter_bytes; i++){
    int j = 8*(seconds_counter_bytes + fractional_counter_bytes - i - 1);
    //printf("shift %d bits\n", j);
    //printf("writing %d at index %d \n", (uint8_t)(tai_counter >> j), i0 + i);
    r[i0 + i] = tai_counter >> j;
  }

  return r;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const UTC_Timestamp Instant<seconds_counter_bytes, fractional_counter_bytes>::as_UTC_timestamp(){
  return UTC_Timestamp();
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
