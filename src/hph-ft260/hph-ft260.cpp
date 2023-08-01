#include "hph-ft260.hpp"

namespace hph
{

   uchar ft260_interface::numbered_gpio_map[ft260_gpio_max] = {gpio_0, gpio_1, gpio_2, gpio_3, gpio_4, gpio_5};
   uchar ft260_interface::lettered_gpio_map[ft260_gpio_extra_max] = {gpio_a, gpio_b, gpio_c, gpio_d, gpio_e, gpio_f, gpio_g, gpio_h};

   ft260_interface::ft260_interface(const char *device_paths_in[], int **error_code_out)
      : device_paths(device_paths_in), error_code(error_code_out)
   {
      devices = -1;
      int hid_devices = 0;

      int *temporary_error_codes;
      int temporary_error_code_counter = 0;

      hid_device** temporary_handles;

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
         *error_code = (int*) calloc(1, sizeof(int));
         *error_code[0] = 1; // hid_init failure error
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

         devices_found = (char**) calloc(total_devices, sizeof(char*));
         hid_devices = get_devices(devs, device_parameters, devices_found, &corresponding_interface_number);

         printf("ft260 hid devices found is %d\n", hid_devices);

         temporary_error_codes = (int *) calloc(hid_devices, sizeof(int));
         handles = (hid_device**) calloc(hid_devices, sizeof(hid_device*));

         if(strcmp(device_paths[0],"all") != 0)
         {
            while(strcmp(device_paths[devices_to_be_opened],"") != 0)
            {
               ++devices_to_be_opened;
            }

            if(devices_to_be_opened != 0)
            {
               int handle_number = 0;
               for(int i = 0; i < hid_devices; ++i)
               {
                  bool device_found = find_device(i);

                  if(device_found)
                  {
                     printf("\n");
                     printf("ft260 device %d is handle %d and path is %s\n", i, handle_number, devices_found[i]);
                     temporary_error_codes[i] = open_device(handle_number++, i);
                  }
                  else
                  {
                     //signify that this device is not used
                     temporary_error_codes[i] = -1;
                  }
               }
            }
         }
         else
         {
            initialize_gpio(hid_devices);
            for(int i = 0; i < hid_devices; ++i)
            {
               printf("\n");
               printf("ft260 device %d path is %s\n", i, devices_found[i]);
               temporary_error_codes[i] = open_device(i, i);
            }
         }

         printf("\n");
         hid_free_enumeration(devs);
      }

      if(devices_to_be_opened_found != devices_to_be_opened)
      {
         int devices_to_be_opened_found_check = 0;

         for(int i = 0; i < hid_devices; ++i)
         {
            devices_to_be_opened_found_check += ((temporary_error_codes[i] != -1) ? 1 : 0);
         }

         if(devices_to_be_opened_found_check != devices_to_be_opened_found)
         {
            printf("UNEXPECTED ERROR, EXITING\n");
            exit(0);
         }

         initialize_gpio(devices_to_be_opened_found_check);

         //need loop to initialize from temp values
         //*error_code = (int*) calloc(devices_to_be_opened_found_check, sizeof(int));
         //handles = (hid_device**) calloc(hid_devices, sizeof(hid_device*));


         for(int i = 0; i < hid_devices; ++i)
         {
            (*error_code)[i] = 5; // hid_device to be opened not found
         }
         printf("DEVICES THAT WERE INTENDED TO BE OPENED WERE NOT FOUND!!!\n");
         printf("WE WILL NOT ABORT BUT INTENDED EFFECT IS HIGHLY LIKELY NOT TO BE ACCOMPLISHED.\n");
      }
      else
      {
         //need loop to initialize from temp values
         //*error_code = (int*) calloc(devices_to_be_opened_found_check, sizeof(int));
         //handles = (hid_device**) calloc(hid_devices, sizeof(hid_device*));
         initialize_gpio(hid_devices);
      }

      buffer_slots_used = 0;

      free(temporary_error_codes);
      free(temporary_handles);
   }

   ft260_interface::~ft260_interface(void)
   {
      free(error_code);

      for(int i = 0; i < devices; ++i)
      {
         hid_close(handles[i]);
      }
      free(handles);
      for(int i = 0; i < total_devices; ++i)
      {
         free(devices_found[i]);
      }
      free(devices_found);

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

   int ft260_interface::open_device(uint8_t device_handle, uint8_t device_index)
   {
      int local_error_code = -1;
      handles[device_handle] = hid_open_path(devices_found[device_index]);

      if (!handles[device_handle])
      {
         printf("Unable to open device\n");
         hid_exit();
         local_error_code = 2; // hid_device open failure error
      }
      else
      {
         printf("interface number %d\n", corresponding_interface_number[device_index]);

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
            local_error_code = 3; // hid_device unable to change blocking state error
         }

         add_to_buffer(chip_version);
         if(read_data(device_handle) < 0)
         {
            local_error_code = 4; // hid_device unable to read data error
         }
         reset_active_buffer();


         if(set_as_blocking(device_handle) < 0)
         {
            local_error_code = 3; // hid_device unable to change blocking state error
         }
      }

      return local_error_code;
   }

   bool ft260_interface::find_device(uint8_t device_handle)
   {
      bool device_found = false;
      uint8_t test_device = 0;
      while(!device_found)
      {
         if(strcmp(device_paths[test_device], devices_found[device_handle]) == 0)
         {
            ++devices_to_be_opened_found;
            device_found = true;
         }
         else
         {
            ++test_device;

            if(test_device >= devices_to_be_opened)
            {
               //registering an error code makes no sense as there is no
               //handle to associate said error with so just print the
               //issue
               printf("\n");
               printf("ft260 device %d at path %s is not in list of "
                      "devices to opened, so although it is available "
                      "it will not be opened\n", device_handle, devices_found[device_handle]);
               break;
            }
         }
      }

      return device_found;
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

   uint8_t ft260_interface::i2c_data_report_id(uint8_t len)
   {
      return (i2c_report_min + ((len)-1) / 4);
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
