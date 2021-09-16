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
Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(etl::array<uint8_t, 9> timestamp){
  tai_counter = 0; //TODO
}

// template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
// Instant<seconds_counter_bytes, fractional_counter_bytes>::Instant(UTC_Timestamp timestamp){
//   tai_counter = 0;//(tai_counter_t)seconds << 8*fractional_counter_bytes;
// }

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
