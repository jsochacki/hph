#ifndef MAX11603EEE_LIBRARY_H_
#define MAX11603EEE_LIBRARY_H_

#include <cstdint>
#include <cmath>

namespace max11603eee
{
   constexpr double max11603eee_voltage_reference = 3.2;  //4.7 is most accurate
   constexpr double max11603eee_bit_depth = 8;
   constexpr double max11603eee_volts_per_bits = max11603eee_voltage_reference / pow(float(2), float(max11603eee_bit_depth));

   const uint8_t max11603eee_address = 0b01101101;

   const uint8_t setup_write = 0b10000000;
   const uint8_t use_external_reference = 0b00100000;
   const uint8_t use_external_clock = 0b00001000;
   const uint8_t use_unipolar = 0b00000000;
   const uint8_t dont_reset_configuration = 0b00000010;

   const uint8_t configuration_write = 0b00000000;
   const uint8_t no_scan =   0b01100000;
   const uint8_t unused_cs = 0b00000000;
   const uint8_t channel_0 = 0b00000000;
   const uint8_t channel_1 = 0b00000010;
   const uint8_t channel_2 = 0b00000100;
   const uint8_t channel_3 = 0b00000110;
   const uint8_t channel_4 = 0b00001000;
   const uint8_t channel_5 = 0b00001010;
   const uint8_t channel_6 = 0b00001100;
   const uint8_t channel_7 = 0b00001110;
   const uint8_t use_single_ended = 0b00000001;

   const uint8_t setup_value
     = setup_write
     | use_external_reference
     | use_external_clock
     | use_unipolar
     | dont_reset_configuration;

   const uint8_t base_configuration_value
     = configuration_write
     | no_scan
     | unused_cs
     | use_single_ended;

   const uint8_t vset_value = 0b11010110;

}   // namespace max11603eee

#endif /* MAX11603EEE_LIBRARY_H_ */
