#include "hph-ft260-mnc-functions.hpp"

int main(int argc, char* argv[])
{
   int result_size;

   int device_count;
   std::vector<int> device_error_codes;
   std::vector<std::string> device_paths_to_open;

   //device_paths_to_open.emplace_back("errorslol");
   //device_paths_to_open.emplace_back("2-2.3:1.0");
   //device_paths_to_open.emplace_back("2-2.2:1.1");
   //device_paths_to_open.emplace_back("3-1.3:1.0");
   device_paths_to_open.emplace_back("3-1.2:1.1");
   //device_paths_to_open.emplace_back("all");
   //device_paths_to_open.emplace_back("");

   hph::ft260_interface ft260s(device_paths_to_open);

   hph::mnc::check_interface(ft260s);
   hph::mnc::check_errors(ft260s, device_error_codes);
   hph::mnc::display_active_paths(ft260s);
   hph::mnc::initialize_device_for_non_hid_i2c_and_gpio(ft260s, 0);

   hph::mnc::reset_ethernet_chip(ft260s, 0);

   return 0;
}
