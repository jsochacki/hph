#ifndef MAX11603EEE_LIBRARY_H_
#define MAX11603EEE_LIBRARY_H_

#include "hph-ft260.hpp"
#include <cstdint>
#include <cmath>

namespace max11603eee
{
   constexpr uint8_t max11603eee_bit_depth = 8;
   constexpr uint8_t max11603eee_minimum_binary_value = 0;
   constexpr uint8_t max11603eee_maximum_binary_value = (1 << max11603eee_bit_depth) - 1;
   constexpr uint16_t max11603eee_binary_range = max11603eee_maximum_binary_value - max11603eee_minimum_binary_value;

   const uint8_t max11603eee_address = 0b01101101;

   enum : uint8_t
   {
      configuration_write = (0b0 << 7),
      setup_write = (0b1 << 7),
      sbf_use_vdd_reference = (0b000 << 4),
      sbf_use_external_reference = (0b010 << 4),
      sbf_use_internal_reference_autoshutdown = (0b100 << 4),
      sbf_use_internal_reference_no_ref_out = (0b101 << 4),
      sbf_use_internal_reference_ref_out = (0b110 << 4),
      sbf_use_internal_clock = (0b0 << 3),
      sbf_use_external_clock = (0b1 << 3),
      sbf_use_unipolar = (0b0 << 2),
      sbf_use_bipolar = (0b1 << 2),
      sbf_dont_reset_configuration = (0b1 << 1),
      sbf_reset_configuration = (0b0 << 1),
   };

   enum : uint8_t
   {
      cbf_no_scan = (0b00 << 5),
      cbf_unused_cs = (0b0 << 4),
      cbf_channel_0 = (0b000 << 1),
      cbf_channel_1 = (0b001 << 1),
      cbf_channel_2 = (0b010 << 1),
      cbf_channel_3 = (0b011 << 1),
      cbf_channel_4 = (0b100 << 1),
      cbf_channel_5 = (0b101 << 1),
      cbf_channel_6 = (0b110 << 1),
      cbf_channel_7 = (0b111 << 1),
      cbf_use_single_ended = 0b1,
      cbf_use_pseudo_differential = 0b0
   };

   const uint8_t default_setup_value
     = setup_write
     | sbf_use_external_reference
     | sbf_use_external_clock
     | sbf_use_unipolar
     | sbf_dont_reset_configuration;

   const uint8_t default_base_configuration_value
     = configuration_write
     | cbf_no_scan
     | cbf_unused_cs
     | cbf_use_single_ended;

   const uint8_t message_packet_size_in_bytes = 2;

   const double default_max11603eee_voltage_reference = 3.2;

   double get_max11603eee_volts_per_bit(double max11603eee_voltage_reference);
   void setup_adc(hph::ft260_interface &ft260s, uint8_t device_handle);
   uint8_t read_adc(hph::ft260_interface &ft260s, uint8_t device_handle, uint8_t channel_number);

}   // namespace max11603eee

#endif /* MAX11603EEE_LIBRARY_H_ */
