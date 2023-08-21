#ifndef DEBUG_H_
#define DEBUG_H_

#include <cstdio>
#include <cstdint>

namespace hph
{
   namespace printing
   {
      namespace binary
      {
         template <typename Type>
         void print_bin(Type value);
      }
   }   // namespace printing
}   // namespace TestBed

namespace hpv = hph::printing::binary;

#endif /* DEBUG_H_ */
