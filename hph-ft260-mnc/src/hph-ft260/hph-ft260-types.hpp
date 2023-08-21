#ifndef HPH_FT260_TYPES_H_
#define HPH_FT260_TYPES_H_

#include <cstdint>   // uint8_t

//#include "hph-hidapi.hpp"
#include <hidapi/hidapi.h>

#include "debug.hpp" // delete when ready
//#define HPH_FT260_PRINT_DEBUG
//#define HPH_FT260_INTERFACE_DEBUG

namespace hph
{
   typedef unsigned char uchar;

   const ushort hph_ft260_max_str_len  = 255;
   const ushort hph_ft260_max_char_buf = 255;
   const ushort hph_ft260_vendor_id    = 0x0403;
   const ushort hph_ft260_product_id   = 0x6030;

   constexpr ushort hph_ft260_max_char_buf_allocation_size = (sizeof(uchar) * hph_ft260_max_char_buf);

   const int device_not_found_index = -1;

   const int device_not_used_error_code = -1;
   const int no_error_error_code = 0;
   const int hid_init_failure_error_code = 1;
   const int hid_device_open_failure_error_code = 2;
   const int hid_device_blocking_state_change_failure_error_code = 3;
   const int hid_device_read_failure_error_code = 4;

   const uint16_t ft260_gpio_max = 6;
   const uint16_t ft260_gpio_extra_max = 8;

   const uint16_t ft260_max_report_length = 64;

   /*
    * The ft260 input report format defines 62 bytes for the data payload,
    * but when requested 62 bytes, the controller returns 60 and 2 in
    * separate input reports. To achieve better performance with the
    * multi-report read data transfers, we set the maximum read payload
    * length to a multiple of 60. With a 100 kHz I2C clock, one 240 bytes
    * read takes about 1/27 second, which is excessive; On the other hand,
    * some higher layer drivers like at24 or optoe limit the i2c reads to
    * 128 bytes. To not block other drivers out of I2C for potentially
    * troublesome amounts of time, we select the maximum read payload length
    * to be 180 bytes.
    */

   const ushort ft260_read_data_max = 180;
   const ushort ft260_write_data_max = 60;

   constexpr uint16_t ft260_gpio_total = (ft260_gpio_max + ft260_gpio_extra_max);
   constexpr uint16_t ft260_gpio_mask = (static_cast<uint16_t>(~(static_cast<uint16_t>(0xffff) << ft260_gpio_total)));

   const uchar read_only_bitmask = 0b00000000;
   const uchar write_only_bitmask = 0b11111111;
   const bool read_only_numbered_gpio_set[ft260_gpio_max] = {false, false, false, false, false, false};
   const bool read_only_lettered_gpio_set[ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
   const bool write_only_numbered_gpio_set[ft260_gpio_max] = {true, true, true, true, true, true};
   const bool write_only_lettered_gpio_set[ft260_gpio_extra_max] = {true, true, true, true, true, true, true, true};

}   // namespace hph

#endif /* HPH_FT260_TYPES_H_ */
