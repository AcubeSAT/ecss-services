template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
int Instant<seconds_counter_bytes, fractional_counter_bytes>::as_TAI_seconds(){
  return tai_counter >> (8*fractional_counter_bytes);
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
void Instant<seconds_counter_bytes, fractional_counter_bytes>::update_from_TAI_seconds(int seconds){
  tai_counter = (tai_counter_t)seconds << 8*fractional_counter_bytes;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
const std::type_info& Instant<seconds_counter_bytes, fractional_counter_bytes>::check_header_type(){
  return typeid(CUC_header);
}
