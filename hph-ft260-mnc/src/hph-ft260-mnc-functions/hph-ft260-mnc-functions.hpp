#ifndef HPH_FT260_MNC_FUNCTIONS_H_
#define HPH_FT260_MNC_FUNCTIONS_H_

#include "hph-ft260.hpp"
#include "isl9122a_library.hpp"
#include "ad5602bksz_library.hpp"
#include "max11603eee_library.hpp"

namespace hph
{
   namespace mnc
   {
      void check_interface(hph::ft260_interface &ft260s);
      void check_errors(ft260_interface &ft260s, std::vector<int> &device_error_codes);
      void display_active_paths(ft260_interface &ft260s);
      void initialize_device_for_non_hid_i2c_and_gpio(ft260_interface &ft260s, uint8_t device_handle);

      void enable_on_fan(ft260_interface &ft260s, uint8_t device_handle);
      void reset_ethernet_chip(ft260_interface &ft260s, uint8_t device_handle);
      void test_i2c(ft260_interface &ft260s, uint8_t device_handle);
   } // namespace mnc
} // namespace hph

#endif /* HPH_FT260_MNC_FUNCTIONS_H_ */
