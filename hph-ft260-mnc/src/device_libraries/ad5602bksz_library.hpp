#ifndef AD5602BKSZ_LIBRARY_H_
#define AD5602BKSZ_LIBRARY_H_

#include <cstdint>
#include <cmath>

namespace ad5602bksz
{
   constexpr double ad5602bksz_voltage_reference = 3.2;
   constexpr double ad5602bksz_bit_depth = 8;
   constexpr double ad5602bksz_bits_per_volt = std::pow(float(2), float(ad5602bksz_bit_depth)) / ad5602bksz_voltage_reference;

   const uint8_t ad5602bksz_address = 0b00001110;

   const uint8_t ad5602bksz_write_reserved_bits = 0b00 << 6;
   const uint8_t ad5602bksz_write_normal_operating_mode = 0b00 << 4;

   const uint8_t ad5602bksz_base_write_value
     = ad5602bksz_write_reserved_bits
     | ad5602bksz_write_normal_operating_mode;

}   // namespace ad5602bksz

#endif /* AD5602BKSZ_LIBRARY_H_ */