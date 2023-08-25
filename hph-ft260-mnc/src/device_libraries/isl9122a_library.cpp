#include "hph-ft260.hpp"
#include "isl9122a_library.hpp"

namespace isl9122a
{
   void initialize_system_fan(hph::ft260_interface &ft260s, uint8_t device_handle)
   {
      int result_size;

      ft260s.add_to_buffer(device_handle, ft260s.i2c_report_min);
      ft260s.add_to_buffer(device_handle, isl9122a::isl9122a_address);
      ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop);
      ft260s.add_to_buffer(device_handle, isl9122a::message_packet_size_in_bytes);
      ft260s.add_to_buffer(device_handle, isl9122a::intflg_mask_address);
      ft260s.add_to_buffer(device_handle, isl9122a::default_intflg_value);
      result_size = ft260s.write_data(device_handle);

      ft260s.add_to_buffer(device_handle, ft260s.i2c_report_min);
      ft260s.add_to_buffer(device_handle, isl9122a::isl9122a_address);
      ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop);
      ft260s.add_to_buffer(device_handle, isl9122a::message_packet_size_in_bytes);
      ft260s.add_to_buffer(device_handle, isl9122a::conv_address);
      ft260s.add_to_buffer(device_handle, isl9122a::default_conv_value);
      result_size = ft260s.write_data(device_handle);
   }

   void set_fan_to_percent(hph::ft260_interface &ft260s, uint8_t device_handle, float percent)
   {
      int result_size;

      uint8_t vset_value = isl9122a::vset_value_min
         + static_cast<uint8_t>(isl9122a::vset_range * (percent / 100.0));

      printf("Setting system fan to value %u for %f percent operation\n",
             vset_value,
             percent);

      ft260s.add_to_buffer(device_handle, ft260s.i2c_report_min);
      ft260s.add_to_buffer(device_handle, isl9122a::isl9122a_address);
      ft260s.add_to_buffer(device_handle, ft260s.flag_start_stop);
      ft260s.add_to_buffer(device_handle, isl9122a::message_packet_size_in_bytes);
      ft260s.add_to_buffer(device_handle, isl9122a::vset_address);
      ft260s.add_to_buffer(device_handle, vset_value);
      result_size = ft260s.write_data(device_handle);
   }
}   // namespace isl9122a
