#include "hph.hpp"
#include <stdlib.h>

int main(int argc, char* argv[])
{
   int result_size;

   int device_count;
   std::vector<std::string> device_paths_to_open;

   //device_paths_to_open.emplace_back("2-2.3:1.0");
   //device_paths_to_open.emplace_back("2-2.2:1.1");
   //device_paths_to_open.emplace_back("all");
   device_paths_to_open.emplace_back("");

   hph::ft260_interface ft260s(device_paths_to_open);

   if(ft260s.ft260_interface_is_alive())
   {
      device_count = ft260s.get_device_count();
      printf("device count is %d\n", device_count);
      if(device_count < 0)
      {
         printf("failed to open any ft260 devices\n");
      }
      else
      {
         // replace with new error code getter
         /*
         for(int i = 0; i < device_count; ++i)
         {
            printf("error code %d is %d\n", i, ft260s_error_codes[i]);
         }
         */
      }

      /*
      ft260s.set_as_non_blocking(0);
      ft260s.set_as_non_blocking(1);
      */

      for(int i = 0; i < device_count; ++i)
      {
         ft260s.set_numbered_gpio_active(i, 0b00000101);
         ft260s.set_lettered_gpio_active(i, 0b00001001);

         hph::uchar nbm = ft260s.get_numbered_gpio_bitmask(i);
         hph::uchar lbm = ft260s.get_lettered_gpio_bitmask(i);

         printf("nbm is %02x\n", nbm);
         printf("lbm is %02x\n", lbm);

         bool nbm_set[hph::ft260_gpio_max] = {true, true, true, false, false, false};
         bool lbm_set[hph::ft260_gpio_extra_max] = {true, false, true, false, true, false, false, false};

         ft260s.set_numbered_gpio_active(i, nbm_set);
         ft260s.set_lettered_gpio_active(i, lbm_set);

         nbm = ft260s.get_numbered_gpio_bitmask(i);
         lbm = ft260s.get_lettered_gpio_bitmask(i);

         printf("nbm is %02x\n", nbm);
         printf("lbm is %02x\n", lbm);
      }

   //
   //   for(int i = 0; i < device_count; ++i)
   //   {
   //      /*
   //      printf("reading chip version for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.chip_version);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //      printf("reading system settings for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //      */
   //
   //      //printf("setting i2c clock speed for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(ft260s.set_i2c_clock_speed);
   //      ft260s.add_to_buffer(ft260s.i2c_clock_speed_100kbps);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("setting i2c mode enabled for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(ft260s.set_i2c_mode);
   //      ft260s.add_to_buffer(ft260s.i2c_mode_enabled);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("setting gpio mode for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(ft260s.set_uart_mode);
   //      ft260s.add_to_buffer(ft260s.mfpin_gpio);
   //      //ft260s.add_to_buffer(0x01);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("setting gpio2 mode for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(ft260s.select_gpio2_func);
   //      ft260s.add_to_buffer(ft260s.mfpin_gpio);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("setting UART DCD RI mode off for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(ft260s.enable_uart_dcd_ri);
   //      ft260s.add_to_buffer(ft260s.mfpin_gpio);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("setting gpioA mode for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(ft260s.select_gpioa_func);
   //      ft260s.add_to_buffer(ft260s.mfpin_gpio);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("setting gpioG mode for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(ft260s.select_gpiog_func);
   //      ft260s.add_to_buffer(ft260s.mfpin_gpio);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("setting suspend output polarity for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(ft260s.set_suspend_out_polar);
   //      ft260s.add_to_buffer(ft260s.suspend_output_active_high);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      /*
   //      //DOESNT WORK!!!!
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      ft260s.add_to_buffer(0x0C);
   //      ft260s.add_to_buffer(0x00);
   //      result_size = ft260s.write_feature_report(i);
   //      */
   //
   //
   //
   //
   //
   //      printf("reading system settings for device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.system_settings);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //
   //      /* GOOD SECTION
   //      sleep(1);
   //
   //      //printf("setting all gpio to inputs on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      ft260s.add_to_buffer(0x00);
   //      ft260s.add_to_buffer(0x10);
   //      ft260s.add_to_buffer(0x00);
   //      ft260s.add_to_buffer(0x10);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("TRYING TO READ GPIO on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //      //printf("setting all gpio to inputs on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      ft260s.add_to_buffer(0x10);
   //      ft260s.add_to_buffer(0x10);
   //      ft260s.add_to_buffer(0x10);
   //      ft260s.add_to_buffer(0x10);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("TRYING TO READ GPIO on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //      sleep(1);
   //
   //      //printf("setting all gpio to inputs on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      ft260s.add_to_buffer(0x00);
   //      ft260s.add_to_buffer(0x10);
   //      ft260s.add_to_buffer(0x00);
   //      ft260s.add_to_buffer(0x10);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("TRYING TO READ GPIO on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //      */
   //
   //
   //      /* BAD SECTION
   //      //printf("setting all gpio to inputs on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      ft260s.add_to_buffer(0x00);
   //      ft260s.add_to_buffer(0x00);
   //      ft260s.add_to_buffer(0x00);
   //      ft260s.add_to_buffer(0x00);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("TRYING TO READ GPIO on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      result_size = ft260s.read_input_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //      */
   //
   //      printf("reading i2c status for device %d\n", i);
   //      ft260s.add_to_buffer(0xC0);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //      printf("reading uart status for device %d\n", i);
   //      ft260s.add_to_buffer(0xE0);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //      sleep(1);
   //
   //      //IT IS GPIOE for CTS on eval board
   //      //GPIO2 is DIO7 and GPIO3 is DIO8 on eval board as well
   //      //Read CTS and GPIO2 while writing GPIO3 high, and then low
   //
   //      //printf("setting all gpio to inputs on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      ft260s.add_to_buffer(0x0C);
   //      ft260s.add_to_buffer(0x08);
   //      ft260s.add_to_buffer(0x10);
   //      ft260s.add_to_buffer(0x00);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("TRYING TO READ GPIO on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //      sleep(1);
   //
   //
   //      //printf("setting all gpio to inputs on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      ft260s.add_to_buffer(0x04);
   //      ft260s.add_to_buffer(0x08);
   //      ft260s.add_to_buffer(0x10);
   //      ft260s.add_to_buffer(0x08);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("TRYING TO READ GPIO on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //
   //      sleep(1);
   //
   //
   //      //IT IS GPIOE for CTS on eval board
   //      //GPIO2 is DIO7 and GPIOF is DIO9 DTR on eval board as well
   //      //Read CTS and DTR while writing GPIO2 high, and then low
   //
   //      //printf("setting all gpio to inputs on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      ft260s.add_to_buffer(0x04);
   //      ft260s.add_to_buffer(0x04);
   //      ft260s.add_to_buffer(0x30);
   //      ft260s.add_to_buffer(0x00);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("TRYING TO READ GPIO on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //      sleep(1);
   //
   //
   //      //printf("setting all gpio to inputs on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      ft260s.add_to_buffer(0x00);
   //      ft260s.add_to_buffer(0x04);
   //      ft260s.add_to_buffer(0x30);
   //      ft260s.add_to_buffer(0x00);
   //      result_size = ft260s.write_feature_report(i);
   //
   //      //printf("TRYING TO READ GPIO on device %d\n", i);
   //      ft260s.add_to_buffer(ft260s.gpio);
   //      result_size = ft260s.read_feature_report(i);
   //      ft260s.print_read_data(result_size);
   //      ft260s.reset_active_buffer();
   //
   //
   //
   //      printf("\n");
   //
   //   }
   //

   }

   return 0;
}
