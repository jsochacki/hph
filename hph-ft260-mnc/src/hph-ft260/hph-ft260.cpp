#include "hph-ft260.hpp"

namespace hph
{

   uchar ft260_interface::numbered_gpio_map[ft260_gpio_max] = {gpio_0, gpio_1, gpio_2, gpio_3, gpio_4, gpio_5};
   uchar ft260_interface::lettered_gpio_map[ft260_gpio_extra_max] = {gpio_a, gpio_b, gpio_c, gpio_d, gpio_e, gpio_f, gpio_g, gpio_h};

   const hid_device *hid_open_path_failure_return_code = NULL;

   ft260_interface::ft260_interface(std::vector<std::string> device_paths_in)
      : device_paths(device_paths_in)
   {
      fatal_errors = false;
      devices = 0;
      int ft260_devices = 0;

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

         printf("number of all hid devices found is %d\n", total_devices);

         ft260_devices = get_devices(devs, device_parameters, devices_found, corresponding_interface_numbers);

         printf("number of ft260 hid devices found is %d\n", ft260_devices);
         printf("\n");


         if(ft260_devices == 0)
         {
            printf("No ft260 devices were found\n");
            printf("Exiting Now\n");
            fatal_errors = true;
         }
         else
         {
            //CANNOT change order of statements below, moving device_paths.empty()
            //check to anywhere but first check produces wrong results since you
            //have to check 0th index entry for other test cases and if it is empty
            //there is none
            if((device_paths.empty())
               || (device_paths[0].compare("") == 0))
            {
               printf("No devices to be opened were specified\n");
               printf("Exiting Now\n");
               fatal_errors = true;
            }
            else if(device_paths[0].compare("all") == 0)
            {
               devices_to_be_opened = ft260_devices;
               device_paths.clear();
               device_paths = devices_found;
            }
            else
            {
               devices_to_be_opened = device_paths.size();
            }

            if(!fatal_errors)
            {
               //need this prior to opening as you do blocking write read test as
               //part of opening
               allocate_active_buffers(ft260_devices);
               initialize_gpio(ft260_devices);
               for(int i = 0; i < ft260_devices; ++i)
               {
                  find_and_open_device(i);
               }
            }
         }

         printf("\n");
         printf("devices to be opened = %d\n", devices_to_be_opened);
         printf("devices to be opened found = %d\n", devices_to_be_opened_found);

         hid_free_enumeration(devs);
      }

      if(fatal_errors)
      {
         deallocate_memory();
      }
      else
      {
         reset_active_buffers(ft260_devices);
         consolidate_used_memory(ft260_devices);
      }
   }

   ft260_interface::~ft260_interface(void)
   {
      deallocate_memory();
      printf("hph-ft260 exited\n");
   }

   void ft260_interface::deallocate_memory(void)
   {
      if(devices > 0)
      {
         printf("\n");
         printf("Deallocating memory and releasing the following devices\n");
         for(int i = 0; i < devices; ++i)
         {
            printf("\n");
            struct hid_device_info* info = hid_get_device_info(handles[i]);
            if (info == NULL)
            {
               printf("Unable to get device info\n");
            }
            else
            {
               print_devices(info);
            }
         }
         for(int i = 0; i < devices; ++i)
         {
            hid_close(handles[i]);
         }

         free_gpio(devices);
      }

      hid_exit();
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
      numbered_gpio_write_notread = (bool**) calloc(device_count, sizeof(bool*));
      lettered_gpio_write_notread = (bool**) calloc(device_count, sizeof(bool*));

      for(uint8_t device_number = 0; device_number < device_count; ++device_number)
      {
         numbered_gpio_active[device_number] = (bool*) calloc(ft260_gpio_max, sizeof(bool));
         lettered_gpio_active[device_number] = (bool*) calloc(ft260_gpio_extra_max, sizeof(bool));
         numbered_gpio_write_notread[device_number] = (bool*) calloc(ft260_gpio_max, sizeof(bool));
         lettered_gpio_write_notread[device_number] = (bool*) calloc(ft260_gpio_extra_max, sizeof(bool));

         for(uint8_t index = 0; index < ft260_gpio_max; ++index)
         {
            numbered_gpio_active[device_number][index] = false;
            numbered_gpio_write_notread[device_number][index] = false;
         }

         for(uint8_t index = 0; index < ft260_gpio_extra_max; ++index)
         {
            lettered_gpio_active[device_number][index] = false;
            lettered_gpio_write_notread[device_number][index] = false;
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
         free(numbered_gpio_write_notread[device_number]);
         free(lettered_gpio_write_notread[device_number]);
      }
      free(numbered_gpio_active);
      free(lettered_gpio_active);
      free(numbered_gpio_write_notread);
      free(lettered_gpio_write_notread);

      free(is_blocking);
   }

   int ft260_interface::open_device(int device_index)
   {
      int local_error_code = no_error_error_code;

      if(device_index == device_not_found_index)
      {
         handles.emplace_back(const_cast<hid_device*>(hid_open_path_failure_return_code));
         local_error_code = device_not_used_error_code;
         return local_error_code;
      }
      // Failure returns NULL
      handles.emplace_back(hid_open_path(devices_found[device_index].c_str()));
      uint8_t device_handle = handles.size() - 1;

      if (handles[device_handle] == hid_open_path_failure_return_code)
      {
         printf("Unable to open device %s\n", devices_found[device_index].c_str());
         local_error_code = hid_device_open_failure_error_code;
      }
      else
      {
         printf("\n");
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

         add_to_buffer(device_handle, chip_version);
         if(read_data(device_handle) < 0)
         {
            local_error_code = hid_device_read_failure_error_code;
         }
         reset_active_buffer(device_handle);


         if(set_as_blocking(device_handle) < 0)
         {
            local_error_code = hid_device_blocking_state_change_failure_error_code;
         }
      }

      return local_error_code;
   }

   void ft260_interface::find_and_open_device(int device_index)
   {
      std::string device_to_find = devices_found[device_index];
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
         device_index = device_not_found_index;
         printf("\n");
         printf("ft260 device %d at path %s is not in list of "
                "devices to opened, so although it is available "
                "it will not be opened\n", device_index, device_to_find.c_str());
      }

      error_codes.emplace_back(open_device(device_index));
   }

   void ft260_interface::consolidate_used_memory(int ft260_devices)
   {
      //Tally up devices used
      for(int i = 0; i < ft260_devices; ++i)
      {
         //Make sure to mark devices we wanted to open but failed to open or
         //perform properly
         if(error_codes[i] != no_error_error_code)
         {
            //device opened but is not working right so close it out first
            if((error_codes[i] == hid_device_blocking_state_change_failure_error_code)
               || (error_codes[i] == hid_device_read_failure_error_code))
            {
               hid_close(handles[i]);
            }
            error_codes[i] = device_not_used_error_code;
         }
         else
         {
            ++devices;
         }
      }

      //number of devices successfully opened, number actually found
      if(devices != devices_to_be_opened)
      {
         //number of devices successfully opened, number actually found
         if(devices != devices_to_be_opened_found)
         {
            printf("\n");
            printf("Devices that were intended to be opened were unable to be opened.\n");
            printf("We will not abort but intended effect is highly likely not to be accomplished.\n");
            printf("\n");

         }
         //number list to be opened, number actually found
         if(devices_to_be_opened != devices_to_be_opened_found)
         {
            printf("\n");
            printf("Devices that were intended to be opened were unable to be found.\n");
            printf("We will not abort but intended effect is highly likely not to be accomplished.\n");
            printf("\n");
         }
      }


      deallocate_active_buffers(ft260_devices);
      allocate_active_buffers(devices);
      free_gpio(ft260_devices);
      initialize_gpio(devices);


      //Have to baby handles and cant treat like rest as we cant risk erasing
      //actual data structures we can only properly create when opening the devices
      int check_index = 0;
      for(std::vector<hid_device*>::iterator it = handles.begin(); it != handles.end();)
      {
         if(error_codes[check_index] == device_not_used_error_code)
         {
            it = handles.erase(it);
         }
         else
         {
            ++it;
         }
         ++check_index;
      }

      //Take care of the rest
      std::vector<int> temporary_error_codes;
      std::vector<std::string> temporary_devices_found;
      std::vector<int> temporary_corresponding_interface_numbers;
      for(int i = 0; i < ft260_devices; ++i)
      {
         if(error_codes[i] != device_not_used_error_code)
         {
            temporary_error_codes.push_back(error_codes[i]);
            temporary_devices_found.push_back(devices_found[i]);
            temporary_corresponding_interface_numbers.push_back(corresponding_interface_numbers[i]);
         }
      }

      error_codes.clear();
      devices_found.clear();
      corresponding_interface_numbers.clear();

      error_codes = temporary_error_codes;
      devices_found = temporary_devices_found;
      corresponding_interface_numbers = temporary_corresponding_interface_numbers;

      for(int i = 0; i < devices; ++i)
      {
         printf("\n");
         printf("ft260 device %d is handle %d and path is %s\n", i, i, devices_found[i].c_str());
      }
      printf("\n");
   }

   uint8_t ft260_interface::i2c_data_report_id(uint8_t len)
   {
      return (i2c_report_min + ((len)-1) / 4);
   }

   void ft260_interface::allocate_active_buffers(int ft260_devices)
   {
      buffer_slots_used = (uint8_t*) calloc(ft260_devices, sizeof(uint8_t));
      active_buffers = (uchar**) calloc(ft260_devices, sizeof(uchar*));
      for(int handle_index = 0; handle_index < ft260_devices; ++handle_index)
      {
         active_buffers[handle_index] = (uchar*) calloc(hph::hph_ft260_max_char_buf, sizeof(uchar));
      }
   }

   void ft260_interface::deallocate_active_buffers(int ft260_devices)
   {
      for(int handle_index = 0; handle_index < ft260_devices; ++handle_index)
      {
         free(active_buffers[handle_index]);
      }
      free(buffer_slots_used);
      free(active_buffers);
   }

   void ft260_interface::reset_active_buffer(uint8_t handle_index)
   {
      buffer_slots_used[handle_index] = 0;
      memset(active_buffers[handle_index], 0, hph::hph_ft260_max_char_buf_allocation_size);
   }

   void ft260_interface::reset_active_buffers(int ft260_devices)
   {
      for(int handle_index = 0; handle_index < ft260_devices; ++handle_index)
      {
         reset_active_buffer(handle_index);
      }
   }

   int ft260_interface::write_data(uint8_t handle_index)
   {
      res = hid_write(handles[handle_index], active_buffers[handle_index], buffer_slots_used[handle_index]);
      if(res < 0)
      {
         printf("unable to write: %ls\n", hid_error(handles[handle_index]));
      }
      else
      {
         reset_active_buffer(handle_index);
      }

      return res;
   }

   int ft260_interface::read_data(uint8_t handle_index)
   {
      memset(active_buffers[handle_index] + 1, 0, hph::hph_ft260_max_char_buf_allocation_size - 1);
      res = hid_read(handles[handle_index], active_buffers[handle_index], hph::hph_ft260_max_char_buf_allocation_size);
      if(res < 0)
      {
         printf("unable to read: %ls\n", hid_error(handles[handle_index]));
         reset_active_buffer(handle_index);
      }

      return res;
   }

   void ft260_interface::print_read_data(uint8_t handle_index, int count)
   {
      for(int index = 0; index < count; ++index)
      {
         printf("%02x ", (unsigned int) active_buffers[handle_index][index]);
      }
      printf("\n");
   }

   int ft260_interface::write_feature_report(uint8_t handle_index)
   {
      //IF YOU DONT RUN WITH PERMISSION YOU WILL GET A SEGMENTATION FAULT
      res = hid_send_feature_report(handles[handle_index], active_buffers[handle_index], buffer_slots_used[handle_index]);
      if(res < 0)
      {
         printf("unable to send feature report %02x: %ls\n", active_buffers[handle_index][0], hid_error(handles[handle_index]));
      }
      else
      {
         reset_active_buffer(handle_index);
      }

      return res;
   }

   int ft260_interface::read_feature_report(uint8_t handle_index)
   {
      memset(active_buffers[handle_index] + 1, 0, hph::hph_ft260_max_char_buf_allocation_size - 1);
      //IF YOU DONT RUN WITH PERMISSION YOU WILL GET A SEGMENTATION FAULT
      res = hid_get_feature_report(handles[handle_index], active_buffers[handle_index], hph::hph_ft260_max_char_buf_allocation_size);
      if(res < 0)
      {
         printf("unable to get feature report %02x: %ls\n", active_buffers[handle_index][0], hid_error(handles[handle_index]));
         reset_active_buffer(handle_index);
      }

      return res;
   }

   int ft260_interface::read_input_report(uint8_t handle_index)
   {
      memset(active_buffers[handle_index] + 1, 0, hph::hph_ft260_max_char_buf_allocation_size - 1);
      //IF YOU DONT RUN WITH PERMISSION YOU WILL GET A SEGMENTATION FAULT
      res = hid_get_input_report(handles[handle_index], active_buffers[handle_index], hph::hph_ft260_max_char_buf_allocation_size);
      if(res < 0)
      {
         printf("unable to get input report %02x: %ls\n", active_buffers[handle_index][0], hid_error(handles[handle_index]));
         reset_active_buffer(handle_index);
      }

      return res;
   }

   void ft260_interface::add_to_buffer(uint8_t handle_index, uchar value)
   {
      active_buffers[handle_index][buffer_slots_used[handle_index]++] = value;
#ifdef HPH_FT260_INTERFACE_DEBUG
      printf("added %02x to buffer slot %d\n", value, buffer_slots_used[handle_index] - 1);
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

   bool ft260_interface::ft260_interface_is_alive(void)
   {
      return (!fatal_errors);
   }

   int ft260_interface::get_device_count(void)
   {
      return devices;
   }

   std::vector<int> ft260_interface::get_error_codes(void)
   {
      return error_codes;
   }

   int ft260_interface::is_device_blocking(uint8_t handle_index)
   {
      return is_blocking[handle_index];
   }

   uchar ft260_interface::get_numbered_gpio_active_bitmask(uint8_t handle_index)
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

   uchar ft260_interface::get_lettered_gpio_active_bitmask(uint8_t handle_index)
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

   uchar ft260_interface::get_numbered_gpio_write_notread_bitmask(uint8_t handle_index)
   {
      uchar bitmask = 0x00;

      for(uint8_t index = 0; index < ft260_gpio_max; ++index)
      {
         if(numbered_gpio_write_notread[handle_index][index])
         {
            bitmask |= numbered_gpio_map[index];
         }
      }

      return bitmask;
   }

   uchar ft260_interface::get_lettered_gpio_write_notread_bitmask(uint8_t handle_index)
   {
      uchar bitmask = 0x00;

      for(uint8_t index = 0; index < ft260_gpio_extra_max; ++index)
      {
         if(lettered_gpio_write_notread[handle_index][index])
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

   void ft260_interface::set_numbered_gpio_write_notread(uint8_t handle_index, uchar bitmask)
   {
      for(uint8_t index = 0; index < ft260_gpio_max; ++index)
      {
         numbered_gpio_write_notread[handle_index][index] = false;
         if((bitmask & (0x01 << index)) != 0)
         {
            numbered_gpio_write_notread[handle_index][index] = true;
         }
      }
   }

   void ft260_interface::set_lettered_gpio_write_notread(uint8_t handle_index, uchar bitmask)
   {
      for(uint8_t index = 0; index < ft260_gpio_extra_max; ++index)
      {
         lettered_gpio_write_notread[handle_index][index] = false;
         if((bitmask & (0x01 << index)) != 0)
         {
            lettered_gpio_write_notread[handle_index][index] = true;
         }
      }
   }

   void ft260_interface::set_numbered_gpio_write_notread(uint8_t handle_index, bool gpio_set[ft260_gpio_max])
   {
      for(uint8_t index = 0; index < ft260_gpio_max; ++index)
      {
         numbered_gpio_write_notread[handle_index][index] = false;
         if(gpio_set[index] == true)
         {
            numbered_gpio_write_notread[handle_index][index] = true;
         }
      }
   }

   void ft260_interface::set_lettered_gpio_write_notread(uint8_t handle_index, bool gpio_set[ft260_gpio_extra_max])
   {
      for(uint8_t index = 0; index < ft260_gpio_extra_max; ++index)
      {
         lettered_gpio_write_notread[handle_index][index] = false;
         if(gpio_set[index] == true)
         {
            lettered_gpio_write_notread[handle_index][index] = true;
         }
      }
   }

   int ft260_interface::read_gpio(uint8_t handle_index)
   {
      reset_active_buffer(handle_index);
      add_to_buffer(handle_index, gpio);
      add_to_buffer(handle_index, get_numbered_gpio_active_bitmask(handle_index));
      add_to_buffer(handle_index, get_numbered_gpio_write_notread_bitmask(handle_index));
      add_to_buffer(handle_index, get_lettered_gpio_active_bitmask(handle_index));
      add_to_buffer(handle_index, get_lettered_gpio_write_notread_bitmask(handle_index));
      return (read_feature_report(handle_index));
   }


   int ft260_interface::write_gpio(uint8_t handle_index)
   {
      reset_active_buffer(handle_index);
      add_to_buffer(handle_index, gpio);
      add_to_buffer(handle_index, get_numbered_gpio_active_bitmask(handle_index));
      add_to_buffer(handle_index, get_numbered_gpio_write_notread_bitmask(handle_index));
      add_to_buffer(handle_index, get_lettered_gpio_active_bitmask(handle_index));
      add_to_buffer(handle_index, get_lettered_gpio_write_notread_bitmask(handle_index));
      return (write_feature_report(handle_index));
   }


   int ft260_interface::read_write_gpio(uint8_t handle_index)
   {
      reset_active_buffer(handle_index);
      add_to_buffer(handle_index, gpio);
      add_to_buffer(handle_index, get_numbered_gpio_active_bitmask(handle_index));
      add_to_buffer(handle_index, get_numbered_gpio_write_notread_bitmask(handle_index));
      add_to_buffer(handle_index, get_lettered_gpio_active_bitmask(handle_index));
      add_to_buffer(handle_index, get_lettered_gpio_write_notread_bitmask(handle_index));
      int write_result = write_feature_report(handle_index);
      int read_result = read_feature_report(handle_index);
      if(write_result != read_result)
      {
         return -1;
      }
      return write_result;
   }


   int ft260_interface::read_gpio(uint8_t handle_index, uchar numbered_bitmask, uchar lettered_bitmask)
   {
      set_numbered_gpio_active(handle_index, numbered_bitmask);
      set_lettered_gpio_active(handle_index, lettered_bitmask);
      set_numbered_gpio_write_notread(handle_index, hph::read_only_bitmask);
      set_lettered_gpio_write_notread(handle_index, hph::read_only_bitmask);
      return (read_gpio(handle_index));
   }

   int ft260_interface::write_gpio(uint8_t handle_index, uchar numbered_bitmask, uchar lettered_bitmask)
   {
      set_numbered_gpio_active(handle_index, numbered_bitmask);
      set_lettered_gpio_active(handle_index, lettered_bitmask);
      set_numbered_gpio_write_notread(handle_index, hph::write_only_bitmask);
      set_lettered_gpio_write_notread(handle_index, hph::write_only_bitmask);
      return (write_gpio(handle_index));
   }

   int ft260_interface::read_gpio(uint8_t handle_index, bool numbered_gpio_set[ft260_gpio_max], bool lettered_gpio_set[ft260_gpio_extra_max])
   {
      set_numbered_gpio_active(handle_index, numbered_gpio_set);
      set_lettered_gpio_active(handle_index, lettered_gpio_set);
      set_numbered_gpio_write_notread(handle_index, const_cast<bool *>(hph::read_only_numbered_gpio_set));
      set_lettered_gpio_write_notread(handle_index, const_cast<bool *>(hph::read_only_lettered_gpio_set));
      return (read_gpio(handle_index));
   }

   int ft260_interface::write_gpio(uint8_t handle_index, bool numbered_gpio_set[ft260_gpio_max], bool lettered_gpio_set[ft260_gpio_extra_max])
   {
      set_numbered_gpio_active(handle_index, numbered_gpio_set);
      set_lettered_gpio_active(handle_index, lettered_gpio_set);
      set_numbered_gpio_write_notread(handle_index, const_cast<bool *>(hph::write_only_numbered_gpio_set));
      set_lettered_gpio_write_notread(handle_index, const_cast<bool *>(hph::write_only_lettered_gpio_set));
      return (write_gpio(handle_index));
   }

   std::vector<std::string> ft260_interface::get_device_paths(void)
   {
      return devices_found;
   }

}
