#include "ad5602bksz_library.hpp"

namespace ad5602bksz
{
   double get_ad5602bksz_bits_per_volt(double ad5602bksz_voltage_reference)
   {
      return (std::pow(float(2), static_cast<float>(ad5602bksz_bit_depth)) / ad5602bksz_voltage_reference);
   }

   uint8_t get_value_from_percent(uint16_t percent)
   {
      return (ad5602bksz_minimum_binary_value
         + static_cast<uint8_t>((ad5602bksz_binary_range * percent) / static_cast<uint16_t>(100U)));
   }

   void generate_dac_bytes(uint16_t percent,
                           uint8_t operating_mode,
                           uint8_t &byte_1,
                           uint8_t &byte_2)
   {
      uint8_t dac_byte_value = get_value_from_percent(percent);

      printf("Setting System LCD Screen backlight to %f V"
             ", binary value %u, for %u percent operation\n",
             (dac_byte_value
             / get_ad5602bksz_bits_per_volt(default_ad5602bksz_voltage_reference)),
             dac_byte_value,
             percent);

      byte_1 = operating_mode | (dac_byte_value >> 4);
      byte_2 = dac_byte_value << 4;
   }

   void set_dac_level_to_percent(hph::ft260_interface &ft260s,
                                 uint8_t device_handle,
                                 uint16_t percent)
   {
      uint8_t byte_1;
      uint8_t byte_2;
      generate_dac_bytes(percent,
                         default_ad5602bksz_base_write_value,
                         byte_1,
                         byte_2);
      write_dac_bus(ft260s, device_handle, byte_1, byte_2);
   }

   void write_dac_bus(hph::ft260_interface &ft260s,
                      uint8_t device_handle,
                      uint8_t byte_1,
                      uint8_t byte_2)
   {
      int result_size;
      uint8_t i2c_report_address
         = ft260s.get_i2c_report_address(ad5602bksz::message_packet_size_in_bytes);

      ft260s.reset_active_buffer(device_handle);

      ft260s.add_to_buffer(device_handle, i2c_report_address);
      ft260s.add_to_buffer(device_handle, ad5602bksz::ad5602bksz_address);
      ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop);
      ft260s.add_to_buffer(device_handle, ad5602bksz::message_packet_size_in_bytes);
      ft260s.add_to_buffer(device_handle, byte_1);
      ft260s.add_to_buffer(device_handle, byte_2);
      result_size = ft260s.write_data(device_handle);
   }
}   // namespace ad5602bksz
