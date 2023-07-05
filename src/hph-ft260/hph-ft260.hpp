#include <cstdint>   // uint8_t

#include <stdio.h>   // printf
#include <string.h>
#include <wchar.h>   // wchar_t

#include "hph-hidapi.hpp"

namespace hph
{
   typedef unsigned char uchar;

   const ushort hph_ft260_max_str_len  = 255;
   const ushort hph_ft260_max_char_buf = 255;
   const ushort hph_ft260_vendor_id    = 0x0403;
   const ushort hph_ft260_product_id   = 0x6030;

   const uint16_t ft260_gpio_max = 6;

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

   const uint16_t ft260_gpio_extra_max = 8;
   constexpr uint16_t ft260_gpio_total = (ft260_gpio_max + ft260_gpio_extra_max);
   constexpr uint16_t ft260_gpio_mask = (static_cast<uint16_t>(~(static_cast<uint16_t>(0xffff) << ft260_gpio_total)));

   enum
   {
      FT260_MODE_ALL  = 0x00,
      FT260_MODE_I2C  = 0x01,
      FT260_MODE_UART = 0x02,
      FT260_MODE_BOTH = 0x03,
   };

   /* Control pipe */
   enum
   {
      FT260_GET_RQST_TYPE = 0xA1,
      FT260_GET_REPORT    = 0x01,
      FT260_SET_RQST_TYPE = 0x21,
      FT260_SET_REPORT    = 0x09,
      FT260_FEATURE       = 0x03,
   };

   /* Report IDs / Feature In */
   enum
   {
      FT260_CHIP_VERSION          = 0xA0,
      FT260_SYSTEM_SETTINGS       = 0xA1,
      FT260_I2C_STATUS            = 0xC0,
      FT260_I2C_READ_REQ          = 0xC2,
      FT260_I2C_REPORT_MIN        = 0xD0,
      FT260_I2C_REPORT_MAX        = 0xDE,
      FT260_GPIO                  = 0xB0,
      FT260_UART_INTERRUPT_STATUS = 0xB1,
      FT260_UART_STATUS           = 0xE0,
      FT260_UART_RI_DCD_STATUS    = 0xE1,
      FT260_UART_REPORT           = 0xF0,
   };

   /* Feature Out */
   enum
   {
      FT260_SET_CLOCK              = 0x01,
      FT260_SET_I2C_MODE           = 0x02,
      FT260_SET_UART_MODE          = 0x03,
      FT260_ENABLE_INTERRUPT       = 0x05,
      FT260_SELECT_GPIO2_FUNC      = 0x06,
      FT260_ENABLE_UART_DCD_RI     = 0x07,
      FT260_SELECT_GPIOA_FUNC      = 0x08,
      FT260_SELECT_GPIOG_FUNC      = 0x09,
      FT260_SET_INTERRUPT_TRIGGER  = 0x0A,
      FT260_SET_SUSPEND_OUT_POLAR  = 0x0B,
      FT260_ENABLE_UART_RI_WAKEUP  = 0x0C,
      FT260_SET_UART_RI_WAKEUP_CFG = 0x0D,
      FT260_SET_I2C_RESET          = 0x20,
      FT260_SET_I2C_CLOCK_SPEED    = 0x22,
      FT260_SET_UART_RESET         = 0x40,
      FT260_SET_UART_CONFIG        = 0x41,
      FT260_SET_UART_BAUD_RATE     = 0x42,
      FT260_SET_UART_DATA_BIT      = 0x43,
      FT260_SET_UART_PARITY        = 0x44,
      FT260_SET_UART_STOP_BIT      = 0x45,
      FT260_SET_UART_BREAKING      = 0x46,
      FT260_SET_UART_XON_XOFF      = 0x49,
   };

   /* Response codes in I2C status report */
   enum
   {
      FT260_I2C_STATUS_SUCCESS     = 0x00,
      FT260_I2C_STATUS_CTRL_BUSY   = 0x01,
      FT260_I2C_STATUS_ERROR       = 0x02,
      FT260_I2C_STATUS_ADDR_NO_ACK = 0x04,
      FT260_I2C_STATUS_DATA_NO_ACK = 0x08,
      FT260_I2C_STATUS_ARBITR_LOST = 0x10,
      FT260_I2C_STATUS_CTRL_IDLE   = 0x20,
      FT260_I2C_STATUS_BUS_BUSY    = 0x40,
   };

   /* I2C Conditions flags */
   enum
   {
      FT260_FLAG_NONE                = 0x00,
      FT260_FLAG_START               = 0x02,
      FT260_FLAG_START_REPEATED      = 0x03,
      FT260_FLAG_STOP                = 0x04,
      FT260_FLAG_START_STOP          = 0x06,
      FT260_FLAG_START_STOP_REPEATED = 0x07,
   };

   /* Multi-function pin functions */
   enum
   {
      FT260_MFPIN_GPIO      = 0x00,
      FT260_MFPIN_SUSPOUT   = 0x01,
      FT260_MFPIN_PWREN     = 0x02,
      FT260_MFPIN_TX_ACTIVE = 0x03,
      FT260_MFPIN_TX_LED    = 0x04,
      FT260_MFPIN_RX_LED    = 0x05,
      FT260_MFPIN_BCD_DET   = 0x06,
   };

   enum
   {
      FT260_GPIO_VALUE      = 0x00,
      FT260_GPIO_DIRECTION  = 0x01,
      FT260_GPIO_DIR_INPUT  = 0x00,
      FT260_GPIO_DIR_OUTPUT = 0x01,
   };

   /* GPIO offsets */
   enum
   {
      FT260_GPIO_0 = (1 << 0),
      FT260_GPIO_1 = (1 << 1),
      FT260_GPIO_2 = (1 << 2),
      FT260_GPIO_3 = (1 << 3),
      FT260_GPIO_4 = (1 << 4),
      FT260_GPIO_5 = (1 << 5),
      FT260_GPIO_A = (1 << (ft260_gpio_max + 0)),
      FT260_GPIO_B = (1 << (ft260_gpio_max + 1)),
      FT260_GPIO_C = (1 << (ft260_gpio_max + 2)),
      FT260_GPIO_D = (1 << (ft260_gpio_max + 3)),
      FT260_GPIO_E = (1 << (ft260_gpio_max + 4)),
      FT260_GPIO_F = (1 << (ft260_gpio_max + 5)),
      FT260_GPIO_G = (1 << (ft260_gpio_max + 6)),
      FT260_GPIO_H = (1 << (ft260_gpio_max + 7)),
   };

   /* GPIO groups */
   enum
   {
      FT260_GPIO_WAKEUP       = (FT260_GPIO_3),
      FT260_GPIO_I2C_DEFAULT  = (FT260_GPIO_0 | FT260_GPIO_1),
      FT260_GPIO_UART_DCD_RI  = (FT260_GPIO_4 | FT260_GPIO_5),
      FT260_GPIO_UART         = (FT260_GPIO_B | FT260_GPIO_C | FT260_GPIO_D
                         | FT260_GPIO_E | FT260_GPIO_F | FT260_GPIO_H),
      FT260_GPIO_UART_DEFAULT = (FT260_GPIO_UART | FT260_GPIO_UART_DCD_RI),
   };

   enum
   {
      FT260_I2C_CLOCK_12MHZ  = 0x00,
      FT260_I2C_CLOCK_24MHZ  = 0x01,
      FT260_I2C_CLOCK_48MHZ  = 0x02,
   };


   uint8_t constexpr ft260_i2c_data_report_id(uint8_t len)
   {
      return (FT260_I2C_REPORT_MIN + ((len)-1) / 4);
   }

   class ft260_interface
   {
      public:
      ft260_interface();
      ft260_interface(int* error_code_out);

      int initialize_ft260_as_gpio(uint8_t handle_index);
      int write_system_setting(uint8_t handle_index);

      private:
      int res;
      int total_devices;
      int ft260_devices;

      unsigned char buf[hph_ft260_max_char_buf];

      wchar_t wstr[hph_ft260_max_str_len];

      struct hid_device_info* devs;

      hid_device** handles;


      hid_device_search_parameters ft260_device_parameters;

      int* error_code;
   };

}   // namespace hph
