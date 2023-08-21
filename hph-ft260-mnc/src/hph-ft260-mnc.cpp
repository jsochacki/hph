#include "hph-ft260-mnc.hpp"
#include <stdlib.h>

int main(int argc, char* argv[])
{
   int result_size;

   int device_count;
   std::vector<int> device_error_codes;
   std::vector<std::string> device_paths_to_open;

   //device_paths_to_open.emplace_back("errorslol");
   device_paths_to_open.emplace_back("2-2.3:1.0");
   device_paths_to_open.emplace_back("2-2.2:1.1");
   //device_paths_to_open.emplace_back("3-1.3:1.0");
   //device_paths_to_open.emplace_back("3-1.2:1.1");
   //device_paths_to_open.emplace_back("all");
   //device_paths_to_open.emplace_back("");

   hph::ft260_interface ft260s(device_paths_to_open);

   if(!ft260s.ft260_interface_is_alive())
   {
      exit(0);
   }

   device_count = ft260s.get_device_count();
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

   /*
   ft260s.set_as_non_blocking(0);
   ft260s.set_as_non_blocking(1);
   */

   std::vector<std::string> active_device_paths = ft260s.get_device_paths();
   for(int i = 0; i < device_count; ++i)
   {
      printf("get device paths for handle %d is %s\n", i, active_device_paths[i].c_str());
   }

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

   {
      int i = 1;
      printf("reading chip version for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.chip_version);
      result_size = ft260s.read_feature_report(i);
      ft260s.print_read_data(i, result_size);
      ft260s.reset_active_buffer(i);

      printf("reading system settings for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      result_size = ft260s.read_feature_report(i);
      ft260s.print_read_data(i, result_size);
      ft260s.reset_active_buffer(i);

      //printf("setting i2c clock speed for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      ft260s.add_to_buffer(i, ft260s.set_i2c_clock_speed);
      ft260s.add_to_buffer(i, ft260s.i2c_clock_speed_100kbps);
      result_size = ft260s.write_feature_report(i);

      //printf("setting i2c mode enabled for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      ft260s.add_to_buffer(i, ft260s.set_i2c_mode);
      ft260s.add_to_buffer(i, ft260s.i2c_mode_disabled);
      result_size = ft260s.write_feature_report(i);

      //printf("setting gpio mode for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      ft260s.add_to_buffer(i, ft260s.set_uart_mode);
      ft260s.add_to_buffer(i, ft260s.mfpin_gpio);
      //ft260s.add_to_buffer(i, 0x01);
      result_size = ft260s.write_feature_report(i);

      //printf("setting gpio2 mode for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      ft260s.add_to_buffer(i, ft260s.select_gpio2_func);
      ft260s.add_to_buffer(i, ft260s.mfpin_gpio);
      result_size = ft260s.write_feature_report(i);

      //printf("setting UART DCD RI mode off for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      ft260s.add_to_buffer(i, ft260s.enable_uart_dcd_ri);
      ft260s.add_to_buffer(i, ft260s.uart_ri_wakeup_disabled);
      result_size = ft260s.write_feature_report(i);

      //printf("setting gpioA mode for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      ft260s.add_to_buffer(i, ft260s.select_gpioa_func);
      ft260s.add_to_buffer(i, ft260s.mfpin_gpio);
      result_size = ft260s.write_feature_report(i);

      //printf("setting gpioG mode for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      ft260s.add_to_buffer(i, ft260s.select_gpiog_func);
      ft260s.add_to_buffer(i, ft260s.mfpin_gpio);
      result_size = ft260s.write_feature_report(i);

      //printf("setting suspend output polarity for device %d\n", i);
      ft260s.add_to_buffer(i, ft260s.system_settings);
      ft260s.add_to_buffer(i, ft260s.set_suspend_out_polar);
      ft260s.add_to_buffer(i, ft260s.suspend_output_active_high);
      result_size = ft260s.write_feature_report(i);


      {
      //Write gpio2 on eval board high then low
      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, false, false, false};
      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
      bool nbm_wnr[hph::ft260_gpio_max] = {true, false, false, false, false, false};
      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};

      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
      ft260s.print_read_data(i, result_size);
      }

      sleep(1);

      {
      //Write gpio2 on eval board high then low
      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, false, false, false};
      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
      bool nbm_wnr[hph::ft260_gpio_max] = {false, true, false, false, false, false};
      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};

      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
      ft260s.print_read_data(i, result_size);
      }

      sleep(1);

      {
      //Write gpio2 on eval board high then low
      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, false, false, false};
      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, false, false, false};
      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};

      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
      ft260s.print_read_data(i, result_size);
      }

      sleep(1);

      {
      //Write gpio2 on eval board high then low
      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, true, false, false};
      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, true, false, false};
      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};

      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
      ft260s.print_read_data(i, result_size);
      }
   }


//
//   for(int i = 0; i < device_count; ++i)
//   {
//      printf("reading chip version for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.chip_version);
//      result_size = ft260s.read_feature_report(i);
//      ft260s.print_read_data(i, result_size);
//      ft260s.reset_active_buffer(i);
//
//      printf("reading system settings for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      result_size = ft260s.read_feature_report(i);
//      ft260s.print_read_data(i, result_size);
//      ft260s.reset_active_buffer(i);
//
//      //printf("setting i2c clock speed for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      ft260s.add_to_buffer(i, ft260s.set_i2c_clock_speed);
//      ft260s.add_to_buffer(i, ft260s.i2c_clock_speed_100kbps);
//      result_size = ft260s.write_feature_report(i);
//
//      //printf("setting i2c mode enabled for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      ft260s.add_to_buffer(i, ft260s.set_i2c_mode);
//      ft260s.add_to_buffer(i, ft260s.i2c_mode_enabled);
//      result_size = ft260s.write_feature_report(i);
//
//      //printf("setting gpio mode for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      ft260s.add_to_buffer(i, ft260s.set_uart_mode);
//      ft260s.add_to_buffer(i, ft260s.mfpin_gpio);
//      //ft260s.add_to_buffer(i, 0x01);
//      result_size = ft260s.write_feature_report(i);
//
//      //printf("setting gpio2 mode for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      ft260s.add_to_buffer(i, ft260s.select_gpio2_func);
//      ft260s.add_to_buffer(i, ft260s.mfpin_gpio);
//      result_size = ft260s.write_feature_report(i);
//
//      //printf("setting UART DCD RI mode off for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      ft260s.add_to_buffer(i, ft260s.enable_uart_dcd_ri);
//      ft260s.add_to_buffer(i, ft260s.uart_ri_wakeup_disabled);
//      result_size = ft260s.write_feature_report(i);
//
//      //printf("setting gpioA mode for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      ft260s.add_to_buffer(i, ft260s.select_gpioa_func);
//      ft260s.add_to_buffer(i, ft260s.mfpin_gpio);
//      result_size = ft260s.write_feature_report(i);
//
//      //printf("setting gpioG mode for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      ft260s.add_to_buffer(i, ft260s.select_gpiog_func);
//      ft260s.add_to_buffer(i, ft260s.mfpin_gpio);
//      result_size = ft260s.write_feature_report(i);
//
//      //printf("setting suspend output polarity for device %d\n", i);
//      ft260s.add_to_buffer(i, ft260s.system_settings);
//      ft260s.add_to_buffer(i, ft260s.set_suspend_out_polar);
//      ft260s.add_to_buffer(i, ft260s.suspend_output_active_high);
//      result_size = ft260s.write_feature_report(i);
//
//
//      /*
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {true, false, false, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.print_read_data(i, result_size);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, true, false, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.print_read_data(i, result_size);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.print_read_data(i, result_size);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, false, true, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.print_read_data(i, result_size);
//      ft260s.reset_active_buffer(i);
//      }
//      */
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {true, false, false, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, true, false, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, true, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, false, true, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, true, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.reset_active_buffer(i);
//      }
//
//      /*
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, true, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.print_read_data(i, result_size);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, true, false, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, true, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.write_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//
//      {
//      //Write gpio2 on eval board high then low
//      bool nbm_active[hph::ft260_gpio_max] = {false, false, false, true, false, false};
//      bool lbm_active[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//      bool nbm_wnr[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_wnr[hph::ft260_gpio_extra_max] = {false, false, false, false, false, false, false, false};
//
//      result_size = ft260s.read_gpio(i, nbm_active, lbm_active, nbm_wnr, lbm_wnr);
//      ft260s.print_read_data(i, result_size);
//      ft260s.reset_active_buffer(i);
//      }
//
//      sleep(1);
//      */
//
//
//      /*
//      //Ethernet reset not is GPIOE in hph
//      bool nbm_set[hph::ft260_gpio_max] = {false, false, false, false, false, false};
//      bool lbm_set[hph::ft260_gpio_extra_max] = {false, false, false, false, true, false, false, false};
//
//      ft260s.write_gpio(i, nbm_set, lbm_set);
//      */
//
///*
//      ft260s.set_numbered_gpio_write_notread(i, 0b00000101);
//      ft260s.set_lettered_gpio_write_notread(i, 0b00001001);
//
//      hph::uchar nbm = ft260s.get_numbered_gpio_bitmask(i);
//      hph::uchar lbm = ft260s.get_lettered_gpio_bitmask(i);
//
//      printf("nbm is %02x\n", nbm);
//      printf("lbm is %02x\n", lbm);
//
//      bool nbm_set[hph::ft260_gpio_max] = {true, true, true, false, false, false};
//      bool lbm_set[hph::ft260_gpio_extra_max] = {true, false, true, false, true, false, false, false};
//
//      ft260s.set_numbered_gpio_write_notread(i, nbm_set);
//      ft260s.set_lettered_gpio_write_notread(i, lbm_set);
//
//      nbm = ft260s.get_numbered_gpio_bitmask(i);
//      lbm = ft260s.get_lettered_gpio_bitmask(i);
//
//      printf("nbm is %02x\n", nbm);
//      printf("lbm is %02x\n", lbm);
//      */
// 
//
//
//
////      //IT IS GPIOE for CTS on eval board
////      //GPIO2 is DIO7 and GPIOF is DIO9 DTR on eval board as well
////      //Read CTS and DTR while writing GPIO2 high, and then low
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x04);
////      ft260s.add_to_buffer(i, 0x04);
////      ft260s.add_to_buffer(i, 0x30);
////      ft260s.add_to_buffer(i, 0x00);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////      sleep(1);
////
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x04);
////      ft260s.add_to_buffer(i, 0x30);
////      ft260s.add_to_buffer(i, 0x00);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////
////
////      printf("\n");
//
//
//   }
//
////      /*
////      //DOESNT WORK!!!!
////      ft260s.add_to_buffer(i, ft260s.system_settings);
////      ft260s.add_to_buffer(i, 0x0C);
////      ft260s.add_to_buffer(i, 0x00);
////      result_size = ft260s.write_feature_report(i);
////      */
////
////
////
////
////
////      printf("reading system settings for device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.system_settings);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////
////      /* GOOD SECTION
////      sleep(1);
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x10);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x10);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x10);
////      ft260s.add_to_buffer(i, 0x10);
////      ft260s.add_to_buffer(i, 0x10);
////      ft260s.add_to_buffer(i, 0x10);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////      sleep(1);
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x10);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x10);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////      */
////
////
////      /* BAD SECTION
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x00);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_input_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////      */
////
////      printf("reading i2c status for device %d\n", i);
////      ft260s.add_to_buffer(i, 0xC0);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////      printf("reading uart status for device %d\n", i);
////      ft260s.add_to_buffer(i, 0xE0);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////      sleep(1);
////
////      //IT IS GPIOE for CTS on eval board
////      //GPIO2 is DIO7 and GPIO3 is DIO8 on eval board as well
////      //Read CTS and GPIO2 while writing GPIO3 high, and then low
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x0C);
////      ft260s.add_to_buffer(i, 0x08);
////      ft260s.add_to_buffer(i, 0x10);
////      ft260s.add_to_buffer(i, 0x00);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////      sleep(1);
////
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x04);
////      ft260s.add_to_buffer(i, 0x08);
////      ft260s.add_to_buffer(i, 0x10);
////      ft260s.add_to_buffer(i, 0x08);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////
////      sleep(1);
////
////
////      //IT IS GPIOE for CTS on eval board
////      //GPIO2 is DIO7 and GPIOF is DIO9 DTR on eval board as well
////      //Read CTS and DTR while writing GPIO2 high, and then low
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x04);
////      ft260s.add_to_buffer(i, 0x04);
////      ft260s.add_to_buffer(i, 0x30);
////      ft260s.add_to_buffer(i, 0x00);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////      sleep(1);
////
////
////      //printf("setting all gpio to inputs on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      ft260s.add_to_buffer(i, 0x00);
////      ft260s.add_to_buffer(i, 0x04);
////      ft260s.add_to_buffer(i, 0x30);
////      ft260s.add_to_buffer(i, 0x00);
////      result_size = ft260s.write_feature_report(i);
////
////      //printf("TRYING TO READ GPIO on device %d\n", i);
////      ft260s.add_to_buffer(i, ft260s.gpio);
////      result_size = ft260s.read_feature_report(i);
////      ft260s.print_read_data(i, result_size);
////      ft260s.reset_active_buffer(i);
////
////
////
////      printf("\n");
////
////   }
////

   return 0;
}
