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
  tai_counter = 0; //TODO
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
// TODO


/// ARITHMETIC
// TODO


////////////// TESTS ///////////////
template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const std::type_info& Instant<seconds_counter_bytes, fractional_counter_bytes>::check_header_type(){
  return typeid(CUC_header);
}
