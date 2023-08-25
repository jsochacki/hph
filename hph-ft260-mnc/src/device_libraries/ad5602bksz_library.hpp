#ifndef AD5602BKSZ_LIBRARY_H_
#define AD5602BKSZ_LIBRARY_H_

#include "hph-ft260.hpp"
#include <cstdint>
#include <cmath>

namespace ad5602bksz
{
   constexpr uint8_t ad5602bksz_bit_depth = 8;
   constexpr uint8_t ad5602bksz_minimum_binary_value = 0;
   constexpr uint8_t ad5602bksz_maximum_binary_value = (1 << ad5602bksz_bit_depth) - 1;
   constexpr uint16_t ad5602bksz_binary_range = ad5602bksz_maximum_binary_value - ad5602bksz_minimum_binary_value;

   const uint8_t ad5602bksz_address = 0b00001110;

   enum : uint8_t
   {
      ad5602bksz_write_reserved_bits = (0b00 << 6),
      ad5602bksz_write_normal_operating_mode = (0b00 << 4),
      ad5602bksz_write_power_down_1kohm_load = (0b01 << 4),
      ad5602bksz_write_power_down_100kohm_load = (0b10 << 4),
      ad5602bksz_write_power_down_tri_state_load = (0b11 << 4)
   };

   const uint8_t default_ad5602bksz_base_write_value
     = ad5602bksz_write_reserved_bits
     | ad5602bksz_write_normal_operating_mode;

   const uint8_t message_packet_size_in_bytes = 2;

   const double default_ad5602bksz_voltage_reference = 3.2;

   double get_ad5602bksz_bits_per_volt(double ad5602bksz_voltage_reference);
   uint8_t get_value_from_percent(uint16_t percent);
   void generate_dac_bytes(uint16_t percent,
                           uint8_t operating_mode,
                           uint8_t &byte_1,
                           uint8_t &byte_2);
   void set_dac_level_to_percent(hph::ft260_interface &ft260s,
                                 uint8_t device_handle,
                                 uint16_t percent);
   void write_dac_bus(hph::ft260_interface &ft260s,
                      uint8_t device_handle,
                      uint8_t byte_1,
                      uint8_t byte_2);

}   // namespace ad5602bksz

#endif /* AD5602BKSZ_LIBRARY_H_ */
