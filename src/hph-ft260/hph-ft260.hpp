#include <cstdint> // uint8_t
#include <stdio.h> // printf
#include <wchar.h> // wchar_t
#include <string.h>

#include "hph-hidapi.hpp"

#define HPH_FT260_MAX_STR 255
#define HPH_FT260_MAX_CHAR_BUF 255
#define HPH_FT260_VENDOR_ID 0x0403
#define HPH_FT260_PRODUCT_ID 0x6030

namespace hph
{
   typedef unsigned char uchar;

   uchar ft260_system_setting_code = 0xA1;
}
