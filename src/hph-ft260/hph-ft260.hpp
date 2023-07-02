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

   class ft260_interface
   {
      public:
      ft260_interface();
      ft260_interface(int *error_code_out);

      private:

      int res;
      int total_devices;
      int ft260_devices;

      unsigned char buf[HPH_FT260_MAX_CHAR_BUF];

      wchar_t wstr[HPH_FT260_MAX_STR];

      struct hid_device_info *devs;

      hid_device **handles;


      hid_device_search_parameters ft260_device_parameters;

      uchar ft260_system_setting_code = 0xA1;

      int *error_code;
   };
}
