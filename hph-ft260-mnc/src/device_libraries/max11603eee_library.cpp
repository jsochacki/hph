#include "max11603eee_library.hpp"

namespace max11603eee
{
   double get_max11603eee_volts_per_bit(double max11603eee_voltage_reference)
   {
      return (max11603eee_voltage_reference / std::pow(float(2), static_cast<float>(max11603eee_bit_depth)));
   }

   void setup_adc(hph::ft260_interface &ft260s, uint8_t device_handle)
   {
      int result_size;
      uint8_t i2c_report_address
         = ft260s.get_i2c_report_address(max11603eee::message_packet_size_in_bytes);

      ft260s.reset_active_buffer(device_handle);

      ft260s.add_to_buffer(device_handle, i2c_report_address);
      ft260s.add_to_buffer(device_handle, max11603eee::max11603eee_address);
      ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop);
      ft260s.add_to_buffer(device_handle, max11603eee::message_packet_size_in_bytes);
      ft260s.add_to_buffer(device_handle, max11603eee::default_setup_value);
      ft260s.add_to_buffer(device_handle,
            (max11603eee::default_base_configuration_value
             | max11603eee::cbf_channel_0));
      result_size = ft260s.write_data(device_handle);
   }

   uint8_t read_adc(hph::ft260_interface &ft260s, uint8_t device_handle, uint8_t channel_number)
   {
      int result_size;
      uint8_t i2c_report_address
         = ft260s.get_i2c_report_address(max11603eee::message_packet_size_in_bytes);

      ft260s.reset_active_buffer(device_handle);

      ft260s.add_to_buffer(device_handle, i2c_report_address);
      ft260s.add_to_buffer(device_handle, max11603eee::max11603eee_address);
      ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop);
      ft260s.add_to_buffer(device_handle, max11603eee::message_packet_size_in_bytes);
      ft260s.add_to_buffer(device_handle, max11603eee::default_setup_value);
      ft260s.add_to_buffer(device_handle,
            (max11603eee::default_base_configuration_value | channel_number));
      result_size = ft260s.write_data(device_handle);

      uint8_t bytes_to_read = 1;
      i2c_report_address = ft260s.get_i2c_report_address(bytes_to_read);

      ft260s.add_to_buffer(device_handle, ft260s.i2c_read_req);
      ft260s.add_to_buffer(device_handle, max11603eee::max11603eee_address);
      ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop);
      ft260s.add_to_buffer(device_handle, bytes_to_read);
      ft260s.add_to_buffer(device_handle, 0x00); //MSBs for little endian length
      result_size = ft260s.write_data(device_handle);

      ft260s.reset_active_buffer(device_handle);
      ft260s.add_to_buffer(device_handle, i2c_report_address);
      result_size = ft260s.read_data(device_handle);

      printf("adc result size is %d\n", result_size);
      uint8_t length_of_payload = ft260s.get_data_at_offset(device_handle, 0);
      uint8_t result_value = ft260s.get_data_at_offset(device_handle, length_of_payload);
      printf("adc returned %u bytes with a result value %u is %f\n",
             length_of_payload,
             result_value,
             result_value * get_max11603eee_volts_per_bit(max11603eee::default_max11603eee_voltage_reference));

      ft260s.reset_active_buffer(device_handle);

      return result_value;
   }
}   // namespace max11603eee
