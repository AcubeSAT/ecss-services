#include "Helpers/Instant.hpp"

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
int Instant<seconds_counter_bytes, fractional_counter_bytes>::as_TAI_seconds(){
  return (this -> tai_counter && this -> seconds_mask) >> 8*fractional_counter_bytes;
}

template <uint8_t seconds_counter_bytes, uint8_t fractional_counter_bytes>
void Instant<seconds_counter_bytes, fractional_counter_bytes>::from_TAI_seconds(int seconds){
  this -> tai_counter = (uint64_t)seconds << 8*fractional_counter_bytes;
}
