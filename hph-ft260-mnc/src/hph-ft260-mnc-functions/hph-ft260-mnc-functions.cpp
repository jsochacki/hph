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

         ft260s.reset_active_buffer(device_handle);

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

         printf("setting device clock speed for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_clock);
         ft260s.add_to_buffer(device_handle, ft260s.device_clock_48mhz);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting I2C mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_i2c_mode);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_mode_enabled);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting UART mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_uart_mode);
         ft260s.add_to_buffer(device_handle, ft260s.uart_mode_disabled);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting Interrupt mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.enable_interrupt);
         ft260s.add_to_buffer(device_handle, ft260s.interrupt_disabled);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting gpio2 mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.select_gpio2_func);
         ft260s.add_to_buffer(device_handle, ft260s.mfpin_gpio);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting UART DCD RI mode off for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.enable_uart_dcd_ri);
         ft260s.add_to_buffer(device_handle, ft260s.uart_dcd_ri_disabled);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting gpioA mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.select_gpioa_func);
         ft260s.add_to_buffer(device_handle, ft260s.mfpin_gpio);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting gpioG mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.select_gpiog_func);
         ft260s.add_to_buffer(device_handle, ft260s.mfpin_gpio);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting Interrupt Trigger Condition mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_interrupt_trigger);
         ft260s.add_to_buffer(device_handle, ft260s.interrupt_type_rising_edge);
         ft260s.add_to_buffer(device_handle, ft260s.interrupt_type_level_delay_1ms);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting suspend output polarity for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_suspend_out_polar);
         ft260s.add_to_buffer(device_handle, ft260s.suspend_output_active_high);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting UART RI Wakeup mode for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.enable_uart_ri_wakeup);
         ft260s.add_to_buffer(device_handle, ft260s.uart_ri_wakeup_disabled);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting UART RI Wakeup mode config for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_uart_ri_wakeup_cfg);
         ft260s.add_to_buffer(device_handle, ft260s.uart_ri_wakeup_rising_edge);
         result_size = ft260s.write_feature_report(device_handle);

         printf("setting i2c clock speed for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_i2c_clock_speed);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_clock_speed_100kbps);
         result_size = ft260s.write_feature_report(device_handle);

         printf("reading system settings for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         result_size = ft260s.read_feature_report(device_handle);
         ft260s.print_read_data(device_handle, result_size);
         ft260s.reset_active_buffer(device_handle);
      }

      void enable_on_fan(hph::ft260_interface &ft260s, uint8_t device_handle)
      {
         uchar na = ft260s.get_numbered_gpio_active_bitmask(device_handle);
         uchar la = ft260s.get_lettered_gpio_active_bitmask(device_handle);
         uchar nwnr = ft260s.get_numbered_gpio_write_notread_bitmask(device_handle);
         uchar lwnr = ft260s.get_lettered_gpio_write_notread_bitmask(device_handle);

         na |= ft260s.gpio_5;
         nwnr |= ft260s.gpio_5;

         ft260s.write_gpio(device_handle, na, la, nwnr, lwnr);
      }

      void reset_ethernet_chip(hph::ft260_interface &ft260s, uint8_t device_handle)
      {
         uchar na = ft260s.get_numbered_gpio_active_bitmask(device_handle);
         uchar la = ft260s.get_lettered_gpio_active_bitmask(device_handle);
         uchar nwnr = ft260s.get_numbered_gpio_write_notread_bitmask(device_handle);
         uchar lwnr = ft260s.get_lettered_gpio_write_notread_bitmask(device_handle);

         la &= ~ft260s.gpio_e;
         lwnr |= ft260s.gpio_e;

         ft260s.write_gpio(device_handle, na, la, nwnr, lwnr);

         usleep(1000);
         //takes 2ms with no sleep

         la |= ft260s.gpio_e;

         ft260s.write_gpio(device_handle, na, la, nwnr, lwnr);
      }

      void test_i2c(hph::ft260_interface &ft260s, uint8_t device_handle)
      {
         int result_size;

         ft260s.reset_active_buffer(device_handle);

         /*
         printf("reading I2C status for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_status);
         result_size = ft260s.read_feature_report(device_handle);
         ft260s.print_read_data(device_handle, result_size);
         ft260s.reset_active_buffer(device_handle);

         printf("Resetting I2C Controller %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.system_settings);
         ft260s.add_to_buffer(device_handle, ft260s.set_i2c_reset);
         result_size = ft260s.write_feature_report(device_handle);

         printf("reading I2C status for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_status);
         result_size = ft260s.read_feature_report(device_handle);
         ft260s.print_read_data(device_handle, result_size);
         ft260s.reset_active_buffer(device_handle);
         */

         //check status before trying to transmit

         //uint8_t report_id = ft260s.i2c_data_report_id(1);


         double dac_output_voltage = 1.0;

         uint8_t dac_byte_value = static_cast<uint8_t>(ad5602bksz::ad5602bksz_bits_per_volt * dac_output_voltage);
         uint8_t byte_1 = ad5602bksz::ad5602bksz_base_write_value | (dac_byte_value >> 4);
         uint8_t byte_2 = dac_byte_value << 4;


         printf("test write I2C for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_report_min);
         ft260s.add_to_buffer(device_handle, ad5602bksz::ad5602bksz_address); //address
         ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop); //transaction type
         ft260s.add_to_buffer(device_handle, 0b00000001);  //length
         ft260s.add_to_buffer(device_handle, isl9122a::intflg_address);
         ft260s.add_to_buffer(device_handle, isl9122a::intflg_value);
         result_size = ft260s.write_data(device_handle);


         /*
         printf("test write I2C for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_report_min);
         ft260s.add_to_buffer(device_handle, isl9122a::isl9122a_address); //address
         ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop); //transaction type
         ft260s.add_to_buffer(device_handle, 0b00000001);  //length
         ft260s.add_to_buffer(device_handle, isl9122a::intflg_address);
         ft260s.add_to_buffer(device_handle, isl9122a::intflg_value);
         result_size = ft260s.write_data(device_handle);

         ft260s.add_to_buffer(device_handle, ft260s.i2c_report_min);
         ft260s.add_to_buffer(device_handle, isl9122a::isl9122a_address); //address
         ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop); //transaction type
         ft260s.add_to_buffer(device_handle, 0b00000001);  //length
         ft260s.add_to_buffer(device_handle, isl9122a::conv_address);
         ft260s.add_to_buffer(device_handle, isl9122a::conv_value);
         result_size = ft260s.write_data(device_handle);

         ft260s.add_to_buffer(device_handle, ft260s.i2c_report_min);
         ft260s.add_to_buffer(device_handle, isl9122a::isl9122a_address); //address
         ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop); //transaction type
         ft260s.add_to_buffer(device_handle, 0b00000001);  //length
         ft260s.add_to_buffer(device_handle, isl9122a::vset_address);
         ft260s.add_to_buffer(device_handle, isl9122a::vset_value);
         result_size = ft260s.write_data(device_handle);
         */


         /*
         printf("reading I2C status for device %d\n", device_handle);
         ft260s.add_to_buffer(device_handle, ft260s.i2c_read_req);
         ft260s.add_to_buffer(device_handle, 0b01010101);
         ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop);
         ft260s.add_to_buffer(device_handle, 0b00000001);
         result_size = ft260s.read_data(device_handle);
         ft260s.print_read_data(device_handle, result_size);
         ft260s.reset_active_buffer(device_handle);
         */



/*
         uchar na = ft260s.get_numbered_gpio_active_bitmask(device_handle);
         uchar la = ft260s.get_lettered_gpio_active_bitmask(device_handle);
         uchar nwnr = ft260s.get_numbered_gpio_write_notread_bitmask(device_handle);
         uchar lwnr = ft260s.get_lettered_gpio_write_notread_bitmask(device_handle);

         la &= ~ft260s.gpio_e;
         lwnr |= ft260s.gpio_e;

         ft260s.write_gpio(device_handle, na, la, nwnr, lwnr);

         usleep(1000);
         //takes 2ms with no sleep

         la |= ft260s.gpio_e;

         ft260s.write_gpio(device_handle, na, la, nwnr, lwnr);
         */
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

