#include <cstdint>   // uint8_t

#include <stdio.h>   // printf
#include <string.h>
#include <wchar.h>   // wchar_t
#include <climits>   //for INT_MIN etc...

#include "hph-hidapi.hpp"

//#define HPH_FT260_INTERFACE_DEBUG

namespace hph
{
   typedef unsigned char uchar;

   const ushort hph_ft260_max_str_len  = 255;
   const ushort hph_ft260_max_char_buf = 255;
   const ushort hph_ft260_vendor_id    = 0x0403;
   const ushort hph_ft260_product_id   = 0x6030;

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



   class ft260_interface
   {
      public:

      ft260_interface();
      ft260_interface(const char *device_paths_in[], int **error_code_out);
      ~ft260_interface();

      //int initialize_as_gpio(uint8_t handle_index);

      void reset_active_buffer(void);

      int write_data(uint8_t handle_index);
      int read_data(uint8_t handle_index);
      void print_read_data(int count);

      int write_feature_report(uint8_t handle_index);
      int read_feature_report(uint8_t handle_index);
      int read_input_report(uint8_t handle_index);

      void add_to_buffer(uchar value);

      int set_as_non_blocking(uint8_t handle_index);
      int set_as_blocking(uint8_t handle_index);

      int get_device_count(void);
      int is_device_blocking(uint8_t handle_index);

      uchar get_numbered_gpio_bitmask(uint8_t handle_index);
      uchar get_lettered_gpio_bitmask(uint8_t handle_index);

      void set_numbered_gpio_active(uint8_t handle_index, uchar bitmask);
      void set_lettered_gpio_active(uint8_t handle_index, uchar bitmask);
      void set_numbered_gpio_active(uint8_t handle_index, bool gpio_set[ft260_gpio_max]);
      void set_lettered_gpio_active(uint8_t handle_index, bool gpio_set[ft260_gpio_extra_max]);

      /*
      void read_numbered_gpio_select(uchar bitmask);
      void read_numbered_gpio_value(uchar bitmask);
      void write_numbered_gpio_select(uchar bitmask);
      void write_numbered_gpio_value(uchar bitmask);
      */

      enum : uchar
      {
         mode_all  = 0x00,
         mode_i2c  = 0x01,
         mode_uart = 0x02,
         mode_both = 0x03,
      };

      /* control pipe */
      enum : uchar
      {
         get_rqst_type = 0xa1,
         get_report    = 0x01,
         set_rqst_type = 0x21,
         set_report    = 0x09,
         feature       = 0x03,
      };

      /* report ids / feature in */
      enum : uchar
      {
         chip_version          = 0xa0,
         system_settings       = 0xa1,
         i2c_status            = 0xc0,
         i2c_read_req          = 0xc2,
         i2c_report_min        = 0xd0,
         i2c_report_max        = 0xde,
         gpio                  = 0xb0,
         uart_interrupt_status = 0xb1,
         uart_status           = 0xe0,
         uart_ri_dcd_status    = 0xe1,
         uart_report           = 0xf0,
      };

      /* feature out */
      enum : uchar
      {
         set_clock              = 0x01,
         set_i2c_mode           = 0x02,
         set_uart_mode          = 0x03,
         enable_interrupt       = 0x05,
         select_gpio2_func      = 0x06,
         enable_uart_dcd_ri     = 0x07,
         select_gpioa_func      = 0x08,
         select_gpiog_func      = 0x09,
         set_interrupt_trigger  = 0x0a,
         set_suspend_out_polar  = 0x0b,
         enable_uart_ri_wakeup  = 0x0c,
         set_uart_ri_wakeup_cfg = 0x0d,
         set_i2c_reset          = 0x20,
         set_i2c_clock_speed    = 0x22,
         set_uart_reset         = 0x40,
         set_uart_config        = 0x41,
         set_uart_baud_rate     = 0x42,
         set_uart_data_bit      = 0x43,
         set_uart_parity        = 0x44,
         set_uart_stop_bit      = 0x45,
         set_uart_breaking      = 0x46,
         set_uart_xon_xoff      = 0x49,
      };

      /* response codes in i2c status report */
      enum : uchar
      {
         i2c_status_success     = 0x00,
         i2c_status_ctrl_busy   = 0x01,
         i2c_status_error       = 0x02,
         i2c_status_addr_no_ack = 0x04,
         i2c_status_data_no_ack = 0x08,
         i2c_status_arbitr_lost = 0x10,
         i2c_status_ctrl_idle   = 0x20,
         i2c_status_bus_busy    = 0x40,
      };

      /* i2c conditions flags */
      enum : uchar
      {
         flag_none                = 0x00,
         flag_start               = 0x02,
         flag_start_repeated      = 0x03,
         flag_stop                = 0x04,
         flag_start_stop          = 0x06,
         flag_start_stop_repeated = 0x07,
      };

      /* multi-function pin functions */
      enum : uchar
      {
         mfpin_gpio      = 0x00,
         mfpin_suspout   = 0x01,
         mfpin_pwren     = 0x02,
         mfpin_tx_active = 0x03,
         mfpin_tx_led    = 0x04,
         mfpin_rx_led    = 0x05,
         mfpin_bcd_det   = 0x06,
      };

      enum : uchar
      {
         gpio_value      = 0x00,
         gpio_direction  = 0x01,
         gpio_dir_input  = 0x00,
         gpio_dir_output = 0x01,
      };

      /* GPIO offsets */
      enum : uchar
      {
         gpio_0 = (1 << 0),
         gpio_1 = (1 << 1),
         gpio_2 = (1 << 2),
         gpio_3 = (1 << 3),
         gpio_4 = (1 << 4),
         gpio_5 = (1 << 5)
      };

      enum : uchar
      {
         gpio_a = (1 << 0),
         gpio_b = (1 << 1),
         gpio_c = (1 << 2),
         gpio_d = (1 << 3),
         gpio_e = (1 << 4),
         gpio_f = (1 << 5),
         gpio_g = (1 << 6),
         gpio_h = (1 << 7)
      };

      /* gpio groups */
      enum : uchar
      {
         gpio_wakeup       = (gpio_3),
         gpio_i2c_default  = (gpio_0 | gpio_1),
         gpio_uart_dcd_ri  = (gpio_4 | gpio_5),
         gpio_uart         = (gpio_b | gpio_c | gpio_d
                            | gpio_e | gpio_f | gpio_h),
         gpio_uart_default = (gpio_uart | gpio_uart_dcd_ri),
      };

      enum : uchar
      {
         i2c_clock_12mhz  = 0x00,
         i2c_clock_24mhz  = 0x01,
         i2c_clock_48mhz  = 0x02,
      };

      enum : uchar
      {
         i2c_clock_speed_100kbps  = 0x64,
         i2c_clock_speed_400kbps  = 0x01
      };

      enum : uchar
      {
         i2c_mode_disabled = 0x00,
         i2c_mode_enabled = 0x01
      };

      enum : uchar
      {
         suspend_output_active_high = 0x00,
         suspend_output_active_low = 0x01
      };

      enum : uchar
      {
         uart_ri_wakeup_disabled = 0x00,
         uart_ri_wakeup_enabled = 0x01
      };


      private:

      void hid_api_check(void);
      void initialize_gpio(uint8_t device_count);
      void free_gpio(uint8_t device_count);
      int open_device(uint8_t device_handle, uint8_t device_index);
      bool find_device(uint8_t device_handle);
      void consolidate_used_memory(int hid_devices);

      uint8_t i2c_data_report_id(uint8_t len);

      static uchar numbered_gpio_map[ft260_gpio_max];
      static uchar lettered_gpio_map[ft260_gpio_extra_max];

      int res;
      int total_devices;
      int devices;
      int *corresponding_interface_number;

      unsigned char active_buffer[hph_ft260_max_char_buf];
      uint8_t buffer_slots_used;

      wchar_t wstr[hph_ft260_max_str_len];

      struct hid_device_info* devs;

      hid_device** handles;


      hid_device_search_parameters device_parameters;

      bool *is_blocking;

      char **devices_found;
      const char **device_paths;
      int **error_code;

      bool **numbered_gpio_active;
      bool **lettered_gpio_active;

      uint8_t devices_to_be_opened;
      uint8_t devices_to_be_opened_found;
   };

}   // namespace hph
