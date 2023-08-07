#include "hph-ft260.hpp"

namespace hph
{

   uchar ft260_interface::numbered_gpio_map[ft260_gpio_max] = {gpio_0, gpio_1, gpio_2, gpio_3, gpio_4, gpio_5};
   uchar ft260_interface::lettered_gpio_map[ft260_gpio_extra_max] = {gpio_a, gpio_b, gpio_c, gpio_d, gpio_e, gpio_f, gpio_g, gpio_h};

   ft260_interface::ft260_interface(std::vector<std::string> device_paths_in)
      : device_paths(device_paths_in)
   {
      int hid_devices = 0;

      devices_to_be_opened = 0;
      devices_to_be_opened_found = 0;

      char manufacturer_string[] = "FTDI";
      char product_string[] = "FT260";
      device_parameters.manufacturer_string = utf8_to_wchar_t(manufacturer_string);
      device_parameters.product_string = utf8_to_wchar_t(product_string);
      device_parameters.vendor_id = hph_ft260_vendor_id;
      device_parameters.product_id = hph_ft260_product_id;

      /*
      printf("mfg string is %ls\n", device_parameters.manufacturer_string);
      printf("product string is %ls\n", device_parameters.product_string);
      printf("vendor id is %hu\n", device_parameters.vendor_id);
      printf("product id is %hu\n", device_parameters.product_id);
      */

      printf("NOTE: if the program terminates after throwing an instance of "
             "std::logic_error you didnt run it with sudo\n\n");

      hid_api_check();

      // Initialize the hidapi library
      res = hid_init();
      if(res)
      {
         error_codes.emplace_back(hid_init_failure_error_code); // hid_init failure error
      }
      else
      {
         #if defined(__APPLE__) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
         // To work properly needs to be called before hid_open/hid_open_path after hid_init.
         // Best/recommended option - call it right after hid_init.
         hid_darwin_set_open_exclusive(0);
         #endif


         devs = hid_enumerate(0x0, 0x0);
         total_devices = count_devices(devs);

         #ifdef HPH_FT260_INTERFACE_DEBUG
         print_devices_with_descriptor(devs);
         #endif

         printf("total hid devices found is %d\n", total_devices);

         hid_devices = get_devices(devs, device_parameters, devices_found, corresponding_interface_numbers);

         printf("ft260 hid devices found is %d\n", hid_devices);

         if(device_paths[0].compare("all") == 0)
         {
            initialize_gpio(hid_devices);
            for(int i = 0; i < hid_devices; ++i)
            {
               printf("\n");
               printf("ft260 device %d path is %s\n", i, devices_found[i].c_str());
               error_codes.emplace_back(open_device(i, i));
            }
         }
         else
         {
            devices_to_be_opened = device_paths.size();

            if(devices_to_be_opened != 0)
            {
               initialize_gpio(hid_devices);
               int handle_number = 0;
               for(int i = 0; i < hid_devices; ++i)
               {
                  bool device_found = find_device(i);

                  if(device_found)
                  {
                     printf("\n");
                     printf("ft260 device %d is handle %d and path is %s\n", i, handle_number, devices_found[i].c_str());
                     error_codes.emplace_back(open_device(handle_number++, i));
                  }
                  else
                  {
                     error_codes.emplace_back(device_not_used_error_code);
                  }
               }
            }
         }

         printf("\n");
         hid_free_enumeration(devs);
      }

      if(devices_to_be_opened != devices_to_be_opened_found)
      {
         printf("DEVICES THAT WERE INTENDED TO BE OPENED WERE NOT FOUND!!!\n");
         printf("WE WILL NOT ABORT BUT INTENDED EFFECT IS HIGHLY LIKELY NOT TO BE ACCOMPLISHED.\n");
      }

      consolidate_used_memory(hid_devices);

      buffer_slots_used = 0;
   }

   ft260_interface::~ft260_interface(void)
   {
      printf("devices is %d\n", devices);
      printf("GOT HERE\n");
      for(int i = 0; i < devices; ++i)
      {
         struct hid_device_info* info = hid_get_device_info(handles[i]);
         if (info == NULL)
         {
            printf("Unable to get device info\n");
         }
         else
         {
            print_devices(info);
         }
         printf("GOT HERE\n");
      }
      for(int i = 0; i < devices; ++i)
      {
         printf("GOT HERE\n");
         printf("handles[%d] = %d\n", i, handles[i]);
         hid_close(handles[i]);
      }

      printf("GOT HERE\n");
      free_gpio(devices);

      hid_exit();
      printf("hph-ft260 exited\n");
   }

   /*
   int ft260_interface::initialize_as_gpio(uint8_t handle_index)
   {
      uint8_t index = 0;

      return 0;
   }
   */

   void ft260_interface::hid_api_check(void)
   {
      printf("hph-hidapi. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
      if (HID_API_VERSION == HID_API_MAKE_VERSION(hid_version()->major, hid_version()->minor, hid_version()->patch))
      {
         printf("Compile-time version matches runtime version of hidapi.\n\n");
      }
      else
      {
         printf("Compile-time version is different than runtime version of hidapi.\n]n");
      }
   }

   void ft260_interface::initialize_gpio(uint8_t device_count)
   {
      numbered_gpio_active = (bool**) calloc(device_count, sizeof(bool*));
      lettered_gpio_active = (bool**) calloc(device_count, sizeof(bool*));

      for(uint8_t device_number = 0; device_number < device_count; ++device_number)
      {
         numbered_gpio_active[device_number] = (bool*) calloc(ft260_gpio_max, sizeof(bool));
         lettered_gpio_active[device_number] = (bool*) calloc(ft260_gpio_extra_max, sizeof(bool));

         for(uint8_t index = 0; index < ft260_gpio_max; ++index)
         {
            numbered_gpio_active[device_number][index] = false;
         }

         for(uint8_t index = 0; index < ft260_gpio_extra_max; ++index)
         {
            lettered_gpio_active[device_number][index] = false;
         }
      }

      is_blocking = (bool*) calloc(device_count, sizeof(bool));
   }

   void ft260_interface::free_gpio(uint8_t device_count)
   {
      for(uint8_t device_number = 0; device_number < device_count; ++device_number)
      {
         free(numbered_gpio_active[device_number]);
         free(lettered_gpio_active[device_number]);
      }
      free(numbered_gpio_active);
      free(lettered_gpio_active);

      free(is_blocking);
   }

   int ft260_interface::open_device(uint8_t device_handle, uint8_t device_index)
   {
      int local_error_code = no_error_error_code;
      handles.emplace_back(hid_open_path(devices_found[device_index].c_str()));

      if (!handles[device_handle])
      {
         printf("Unable to open device\n");
         hid_exit();
         local_error_code = hid_device_open_failure_error_code;
      }
      else
      {
         printf("interface number %d\n", corresponding_interface_numbers[device_index]);

         wstr[0] = 0x0000;
         // Read the Manufacturer String
         res = hid_get_manufacturer_string(handles[device_handle], wstr, hph_ft260_max_str_len);
         if (res < 0)
         {
            printf("Unable to read manufacturer string\n");
         }
         printf("Manufacturer String: %ls\n", wstr);

         // Read the Product String
         wstr[0] = 0x0000;
         res = hid_get_product_string(handles[device_handle], wstr, hph_ft260_max_str_len);
         if (res < 0)
         {
            printf("Unable to read product string\n");
         }
         printf("Product String: %ls\n", wstr);

         // Read the Serial Number String
         wstr[0] = 0x0000;
         res = hid_get_serial_number_string(handles[device_handle], wstr, hph_ft260_max_str_len);
         if (res < 0)
         {
            printf("Unable to read serial number string\n");
         }
         printf("Serial Number String: (%d) %ls\n", wstr[0], wstr);

#ifdef HPH_FT260_INTERFACE_DEBUG
         print_hid_report_descriptor_from_device(handles[device_handle]);


         struct hid_device_info* info = hid_get_device_info(handles[device_handle]);
         if (info == NULL)
         {
            printf("Unable to get device info\n");
         }
         else
         {
            print_devices(info);
         }


         // Read Indexed String 1
         wstr[0] = 0x0000;
         res = hid_get_indexed_string(handles[device_handle], 1, wstr, hph_ft260_max_str_len);
         if (res < 0)
         {
            printf("Unable to read indexed string 1\n");
         }
         printf("Indexed String 1: %ls\n", wstr);
#endif
         printf("\n");

         //clear the device buffers
         if(set_as_non_blocking(device_handle) < 0)
         {
            local_error_code = hid_device_blocking_state_change_failure_error_code;
         }

         add_to_buffer(chip_version);
         if(read_data(device_handle) < 0)
         {
            local_error_code = hid_device_read_failure_error_code;
         }
         reset_active_buffer();


         if(set_as_blocking(device_handle) < 0)
         {
            local_error_code = hid_device_blocking_state_change_failure_error_code;
         }
      }

      return local_error_code;
   }

   bool ft260_interface::find_device(uint8_t device_handle)
   {
      std::string device_to_find = devices_found[device_handle];
      bool device_found = false;
      for(auto device_to_check : device_paths)
      {
         if(device_to_check.compare(device_to_find) == 0)
         {
            device_found = true;
            ++devices_to_be_opened_found;
            break;
         }
      }

      if(!device_found)
      {
         //registering an error code makes no sense as there is no
         //handle to associate said error with so just print the
         //issue
         printf("\n");
         printf("ft260 device %d at path %s is not in list of "
                "devices to opened, so although it is available "
                "it will not be opened\n", device_handle, device_to_find.c_str());
      }

      return device_found;
   }

   void ft260_interface::consolidate_used_memory(int hid_devices)
   {
      devices = 0;
      for(int i = 0; i < hid_devices; ++i)
      {
         if(error_codes[i] != device_not_used_error_code)
         {
            ++devices;
         }
      }

      if(devices != devices_to_be_opened_found)
      {
         printf("UNEXPECTED ERROR, EXITING\n");
         exit(0);
      }


      free_gpio(hid_devices);
      initialize_gpio(devices);


      int temp_counter = 0;
      //std::vector<hid_device*> temporary_handles;
      std::vector<int> temporary_error_codes;
      std::vector<std::string> temporary_devices_found;
      std::vector<int> temporary_corresponding_interface_numbers;
      for(int i = 0; i < hid_devices; ++i)
      {
         printf("handles[%d] = %d\n", i, handles[i]);
         if(error_codes[i] != device_not_used_error_code)
         {
            //temporary_handles.push_back(handles[i]);
            printf("handles[%d] = %d\n", i, handles[i]);
            //printf("temporary_handles[%d] = %d\n", temp_counter, temporary_handles[temp_counter]);
            temporary_error_codes.push_back(error_codes[i]);
            temporary_devices_found.push_back(devices_found[i]);
            temporary_corresponding_interface_numbers.push_back(corresponding_interface_numbers[i]);
            ++temp_counter;
         }
      }

      int check_index = 0;
      for(std::vector<hid_device*>::iterator it = handles.begin(); it != handles.end(); ++it)
      {
         printf("it %d is active\n", *it);
         if(error_codes[check_index] == device_not_used_error_code)
         {
            handles.erase(it);
            printf("it %d is being removed\n", *it);
         }
         ++check_index;
      }

      //handles.clear();
      error_codes.clear();
      devices_found.clear();
      corresponding_interface_numbers.clear();

      //handles = temporary_handles;
      error_codes = temporary_error_codes;
      devices_found = temporary_devices_found;
      corresponding_interface_numbers = temporary_corresponding_interface_numbers;

      for(int i = 0; i < devices; ++i)
      {
         printf("handles[%d] = %d\n", i, handles[i]);
         //printf("temporary_handles[%d] = %d\n", i, temporary_handles[i]);
      }
   }

   uint8_t ft260_interface::i2c_data_report_id(uint8_t len)
   {
      return (i2c_report_min + ((len)-1) / 4);
   }

   void ft260_interface::reset_active_buffer(void)
   {
      buffer_slots_used = 0;
      memset(active_buffer, 0, sizeof(active_buffer));
   }

   int ft260_interface::write_data(uint8_t handle_index)
   {
      res = hid_write(handles[handle_index], active_buffer, buffer_slots_used);
      if(res < 0)
      {
         printf("unable to write: %ls\n", hid_error(handles[handle_index]));
      }
      else
      {
         reset_active_buffer();
      }

      return res;
   }

   int ft260_interface::read_data(uint8_t handle_index)
   {
      memset(active_buffer + 1, 0, sizeof(active_buffer) - 1);
      res = hid_read(handles[handle_index], active_buffer, sizeof(active_buffer));
      if(res < 0)
      {
         printf("unable to read: %ls\n", hid_error(handles[handle_index]));
         reset_active_buffer();
      }

      return res;
   }

   void ft260_interface::print_read_data(int count)
   {
      for(int index = 0; index < count; ++index)
      {
         printf("%02x ", (unsigned int) active_buffer[index]);
      }
      printf("\n");
   }

   int ft260_interface::write_feature_report(uint8_t handle_index)
   {
      //IF YOU DONT RUN WITH PERMISSION YOU WILL GET A SEGMENTATION FAULT
      res = hid_send_feature_report(handles[handle_index], active_buffer, buffer_slots_used);
      if(res < 0)
      {
         printf("unable to send feature report %02x: %ls\n", active_buffer[0], hid_error(handles[handle_index]));
      }
      else
      {
         reset_active_buffer();
      }

      return res;
   }

   int ft260_interface::read_feature_report(uint8_t handle_index)
   {
      memset(active_buffer + 1, 0, sizeof(active_buffer) - 1);
      //IF YOU DONT RUN WITH PERMISSION YOU WILL GET A SEGMENTATION FAULT
      res = hid_get_feature_report(handles[handle_index], active_buffer, sizeof(active_buffer));
      if(res < 0)
      {
         printf("unable to get feature report %02x: %ls\n", active_buffer[0], hid_error(handles[handle_index]));
         reset_active_buffer();
      }

      return res;
   }

   int ft260_interface::read_input_report(uint8_t handle_index)
   {
      memset(active_buffer + 1, 0, sizeof(active_buffer) - 1);
      //IF YOU DONT RUN WITH PERMISSION YOU WILL GET A SEGMENTATION FAULT
      res = hid_get_input_report(handles[handle_index], active_buffer, sizeof(active_buffer));
      if(res < 0)
      {
         printf("unable to get input report %02x: %ls\n", active_buffer[0], hid_error(handles[handle_index]));
         reset_active_buffer();
      }

      return res;
   }

   void ft260_interface::add_to_buffer(uchar value)
   {
      active_buffer[buffer_slots_used++] = value;
#ifdef HPH_FT260_INTERFACE_DEBUG
      printf("added %02x to buffer slot %d\n", value, buffer_slots_used - 1);
#endif
   }

   int ft260_interface::set_as_non_blocking(uint8_t handle_index)
   {
      res = hid_set_nonblocking(handles[handle_index], 1);
      if(res < 0)
      {
         printf("unable to set device handle %d to non-blocking because %ls\n",
                handle_index, hid_error(handles[handle_index]));
         return res;
      }

      is_blocking[handle_index] = false;

      return res;
   }

   int ft260_interface::set_as_blocking(uint8_t handle_index)
   {
      res = hid_set_nonblocking(handles[handle_index], 0);
      if(res < 0)
      {
         printf("unable to set device handle %d to blocking because %ls\n",
                handle_index, hid_error(handles[handle_index]));
         return res;
      }

      is_blocking[handle_index] = true;

      return res;
   }

   int ft260_interface::get_device_count(void)
   {
      return devices;
   }

   int ft260_interface::is_device_blocking(uint8_t handle_index)
   {
      return is_blocking[handle_index];
   }

   uchar ft260_interface::get_numbered_gpio_bitmask(uint8_t handle_index)
   {
      uchar bitmask = 0x00;

      for(uint8_t index = 0; index < ft260_gpio_max; ++index)
      {
         if(numbered_gpio_active[handle_index][index])
         {
            bitmask |= numbered_gpio_map[index];
         }
      }

      return bitmask;
   }

   uchar ft260_interface::get_lettered_gpio_bitmask(uint8_t handle_index)
   {
      uchar bitmask = 0x00;

      for(uint8_t index = 0; index < ft260_gpio_extra_max; ++index)
      {
         if(lettered_gpio_active[handle_index][index])
         {
            bitmask |= lettered_gpio_map[index];
         }
      }

      return bitmask;
   }

   void ft260_interface::set_numbered_gpio_active(uint8_t handle_index, uchar bitmask)
   {
      for(uint8_t index = 0; index < ft260_gpio_max; ++index)
      {
         numbered_gpio_active[handle_index][index] = false;
         if((bitmask & (0x01 << index)) != 0)
         {
            numbered_gpio_active[handle_index][index] = true;
         }
      }
   }

   void ft260_interface::set_lettered_gpio_active(uint8_t handle_index, uchar bitmask)
   {
      for(uint8_t index = 0; index < ft260_gpio_extra_max; ++index)
      {
         lettered_gpio_active[handle_index][index] = false;
         if((bitmask & (0x01 << index)) != 0)
         {
            lettered_gpio_active[handle_index][index] = true;
         }
      }
   }

   void ft260_interface::set_numbered_gpio_active(uint8_t handle_index, bool gpio_set[ft260_gpio_max])
   {
      for(uint8_t index = 0; index < ft260_gpio_max; ++index)
      {
         numbered_gpio_active[handle_index][index] = false;
         if(gpio_set[index] == true)
         {
            numbered_gpio_active[handle_index][index] = true;
         }
      }
   }

   void ft260_interface::set_lettered_gpio_active(uint8_t handle_index, bool gpio_set[ft260_gpio_extra_max])
   {
      for(uint8_t index = 0; index < ft260_gpio_extra_max; ++index)
      {
         lettered_gpio_active[handle_index][index] = false;
         if(gpio_set[index] == true)
         {
            lettered_gpio_active[handle_index][index] = true;
         }
      }
   }

   /*
   void ft260_interface::read_numbered_gpio_select(uint8_t handle_index, uchar bitmask)
   {
      for(uint8_t index = 0; index < ft260_gpio_max; ++index)
      {
         numbered_gpio_active[handle_index][index] = false;
         if((bitmask & (0x01 << index)) != 0)
         {
            numbered_gpio_active[handle_index][index] = true;
         }
      }
   }
   */

}
