#include "hph.hpp"

int main(int argc, char* argv[])
{
   int result_size;
   int *ft260s_error_codes;

   int device_count;

   hph::ft260_interface ft260s(&ft260s_error_codes);

   device_count = ft260s.get_device_count();
   printf("device count is %d\n", device_count);
   if(device_count < 0)
   {
      printf("failed to open any ft260 devices\n");
   }
   else
   {
      for(int i = 0; i < device_count; ++i)
      {
         printf("error code %d is %d\n", i, ft260s_error_codes[i]);
      }
   }

   /*
   ft260s.set_as_non_blocking(0);
   ft260s.set_as_non_blocking(1);
   */

   for(int i = 0; i < device_count; ++i)
   {
      printf("reading chip version for device %d\n", i);
      ft260s.add_to_buffer(ft260s.chip_version);
      result_size = ft260s.read_feature_report(i);
      ft260s.print_read_data(result_size);
      ft260s.reset_active_buffer();

      printf("reading system settings for device %d\n", i);
      ft260s.add_to_buffer(ft260s.system_settings);
      result_size = ft260s.read_feature_report(i);
      ft260s.print_read_data(result_size);
      ft260s.reset_active_buffer();

      printf("setting i2c clock speed for device %d\n", i);
      ft260s.add_to_buffer(ft260s.system_settings);
      ft260s.add_to_buffer(ft260s.set_i2c_clock_speed);
      ft260s.add_to_buffer(ft260s.i2c_clock_speed_100kbps);
      result_size = ft260s.write_feature_report(0);

      printf("setting i2c mode enabled for device %d\n", i);
      ft260s.add_to_buffer(ft260s.system_settings);
      ft260s.add_to_buffer(ft260s.set_i2c_mode);
      ft260s.add_to_buffer(ft260s.i2c_mode_enabled);
      result_size = ft260s.write_feature_report(0);

      printf("setting gpio mode for device %d\n", i);
      ft260s.add_to_buffer(ft260s.system_settings);
      ft260s.add_to_buffer(ft260s.set_uart_mode);
      ft260s.add_to_buffer(ft260s.mfpin_gpio);
      result_size = ft260s.write_feature_report(0);

      printf("reading system settings for device %d\n", i);
      ft260s.add_to_buffer(ft260s.system_settings);
      result_size = ft260s.read_feature_report(i);
      ft260s.print_read_data(result_size);
      ft260s.reset_active_buffer();


      printf("setting all gpio to inputs on device %d\n", i);
      ft260s.add_to_buffer(ft260s.gpio);
      ft260s.add_to_buffer(0x00);
      ft260s.add_to_buffer(0x01);
      ft260s.add_to_buffer(0x00);
      ft260s.add_to_buffer(0x01);
      result_size = ft260s.write_feature_report(i);

      printf("TRYING TO READ GPIO on device %d\n", i);
      ft260s.add_to_buffer(ft260s.gpio);
      result_size = ft260s.read_feature_report(i);
      ft260s.print_read_data(result_size);
      ft260s.reset_active_buffer();

   }


   return 0;
}
