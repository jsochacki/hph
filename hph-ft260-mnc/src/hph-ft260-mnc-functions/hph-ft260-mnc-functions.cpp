#include "hph-ft260-mnc-functions.hpp"
#include <stdlib.h>

namespace hph
{
   namespace mnc
   {
      void check_interface(hph::ft260_interface &ft260s)
      {
         if(!(ft260s.ft260_interface_is_alive()))
         {
            exit(0);
         }
      }

      void check_errors(ft260_interface &ft260s, std::vector<int> &device_error_codes)
      {
         int device_count = ft260s.get_device_count();
         printf("device count is %d\n", device_count);
         if(device_count < 0)
         {
            printf("failed to open any ft260 devices\n");
         }
         else
         {
            device_error_codes = ft260s.get_error_codes();
            for(int i = 0; i < device_count; ++i)
            {
               printf("error code %d is %d\n", i, device_error_codes[i]);
            }
         }
         printf("\n");
      }

      void display_active_paths(ft260_interface &ft260s)
      {
         int device_count = ft260s.get_device_count();
         std::vector<std::string> active_device_paths = ft260s.get_device_paths();
         for(int i = 0; i < device_count; ++i)
         {
            printf("device paths for handle %d is %s\n", i, active_device_paths[i].c_str());
         }
      }

      void initialize_device_for_non_hid_i2c_and_gpio(ft260_interface &ft260s, uint8_t device_handle)
      {
         int result_size;

         printf("reading chip version for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.chip_version);
         result_size = ft260s.read_feature_report(device_handle);
         ft260s.print_read_data(device_handle, result_size);
         ft260s.reset_active_buffer(device_handle);

         printf("reading system settings for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         result_size = ft260s.read_feature_report(device_handle);
         ft260s.print_read_data(device_handle, result_size);
         ft260s.reset_active_buffer(device_handle);

         //printf("setting i2c clock speed for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_i2c_clock_speed);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_clock_speed_100kbps);
         result_size = ft260s.write_feature_report(device_handle);

         //printf("setting i2c mode enabled for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_i2c_mode);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_mode_disabled);
         result_size = ft260s.write_feature_report(device_handle);

         //printf("setting gpio mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_uart_mode);
         ft260s.add_to_buffer(device_handle, ft260s.mfpin_gpio);
         //ft260s.add_to_buffer(device_handle, 0x01);
         result_size = ft260s.write_feature_report(device_handle);

         //printf("setting gpio2 mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.select_gpio2_func);
         ft260s.add_to_buffer(device_handle, ft260s.mfpin_gpio);
         result_size = ft260s.write_feature_report(device_handle);

         //printf("setting UART DCD RI mode off for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.enable_uart_dcd_ri);
         ft260s.add_to_buffer(device_handle, ft260s.uart_ri_wakeup_disabled);
         result_size = ft260s.write_feature_report(device_handle);

         //printf("setting gpioA mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.select_gpioa_func);
         ft260s.add_to_buffer(device_handle, ft260s.mfpin_gpio);
         result_size = ft260s.write_feature_report(device_handle);

         //printf("setting gpioG mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.select_gpiog_func);
         ft260s.add_to_buffer(device_handle, ft260s.mfpin_gpio);
         result_size = ft260s.write_feature_report(device_handle);

         //printf("setting suspend output polarity for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_suspend_out_polar);
         ft260s.add_to_buffer(device_handle, ft260s.suspend_output_active_high);
         result_size = ft260s.write_feature_report(device_handle);
      }

      void reset_ethernet_chip(hph::ft260_interface &ft260s, uint8_t device_handle)
      {
         uchar na = ft260s.get_numbered_gpio_active_bitmask(device_handle);
         uchar nwnr = ft260s.get_lettered_gpio_active_bitmask(device_handle);
         uchar la = ft260s.get_numbered_gpio_write_notread_bitmask(device_handle);
         uchar lwnr = ft260s.get_lettered_gpio_write_notread_bitmask(device_handle);

         na |= ft260s.gpio_2;
         nwnr |= ft260s.gpio_2;

         ft260s.write_gpio(device_handle, na, nwnr, la, lwnr);

         sleep(0.1);

         na &= ~ft260s.gpio_2;

         ft260s.write_gpio(device_handle, na, nwnr, la, lwnr);
      }

   } // namespace mnc
} // namespace hph

/*
ft260s.set_as_non_blocking(0);
ft260s.set_as_non_blocking(1);
*/


/*
for(int i = 0; i < device_count; ++i)
{
   ft260s.set_numbered_gpio_active(i, 0b00000101);
   ft260s.set_lettered_gpio_active(i, 0b00001001);

   hph::uchar nbm = ft260s.get_numbered_gpio_active_bitmask(i);
   hph::uchar lbm = ft260s.get_lettered_gpio_active_bitmask(i);

   printf("nbm is %02x\n", nbm);
   printf("lbm is %02x\n", lbm);

   bool nbm_set[hph::ft260_gpio_max] = {true, true, true, false, false, false};
   bool lbm_set[hph::ft260_gpio_extra_max] = {true, false, true, false, true, false, false, false};

   ft260s.set_numbered_gpio_active(i, nbm_set);
   ft260s.set_lettered_gpio_active(i, lbm_set);

   nbm = ft260s.get_numbered_gpio_active_bitmask(i);
   lbm = ft260s.get_lettered_gpio_active_bitmask(i);

   printf("nbm is %02x\n", nbm);
   printf("lbm is %02x\n", lbm);
}
*/

