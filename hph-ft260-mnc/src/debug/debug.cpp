#include "debug.hpp"

namespace hph
{
   namespace printing
   {
      namespace binary
      {
         template <typename Type>
         void print_bin(Type value)
         {
            printf("Bin value ");
            for(int i = ((sizeof(Type) * 8) - 1); i >= 0; i--)
            { printf("%d", (value & (1 << i)) >> i); }
            printf("\n");
         }

         template void print_bin<uint8_t>(uint8_t value);
         template void print_bin<uint16_t>(uint16_t value);

      }   // namespace binary
   }      // namespace printing
}   // namespace hph
