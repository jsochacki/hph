#ifndef ISL9122A_LIBRARY_H_
#define ISL9122A_LIBRARY_H_

#include <cstdint>

namespace isl9122a
{
   const uint8_t isl9122a_address = 0b00011000;

   const uint8_t vset_address = 0x11;
   const uint8_t conv_address = 0x12;
   const uint8_t intflg_address = 0x13;

   const uint8_t vset_value = 0b11010110;
   const uint8_t conv_value = 0b11100001;
   const uint8_t intflg_value = 0b00000000; // ignores en pin

}   // namespace isl9122a

#endif /* ISL9122A_LIBRARY_H_ */
