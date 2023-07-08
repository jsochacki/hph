#include "hph-ft260.hpp"

namespace hph
{
   ft260_interface::ft260_interface(int **error_code_out)
      : error_code(error_code_out)
   {
      devices = -1;

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

      printf("hph-hidapi. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
      if (HID_API_VERSION == HID_API_MAKE_VERSION(hid_version()->major, hid_version()->minor, hid_version()->patch))
      {
         printf("Compile-time version matches runtime version of hidapi.\n\n");
      }
      else
      {
         printf("Compile-time version is different than runtime version of hidapi.\n]n");
      }

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

         char **devices_found = (char**) calloc(total_devices, sizeof(char*));
         devices = get_devices(devs, device_parameters, devices_found, &corresponding_interface_number);

         *error_code = (int*) calloc(devices, sizeof(int));

         printf("ft260 hid devices found is %d\n", devices);

         handles = (hid_device**) calloc(devices, sizeof(hid_device*));
         is_blocking = (bool*) calloc(devices, sizeof(bool));

         for(int i = 0; i < devices; ++i)
         {
            printf("\n");
            printf("ft260 device %d path is %s\n", i, devices_found[i]);

            handles[i] = hid_open_path(devices_found[i]);

            if (!handles[i])
            {
               printf("Unable to open device\n");
               hid_exit();
               (*error_code)[i] = 2; // hid_device open failure error
            }
            else
            {
               printf("interface number %d\n", corresponding_interface_number[i]);

               wstr[0] = 0x0000;
               // Read the Manufacturer String
               res = hid_get_manufacturer_string(handles[i], wstr, hph_ft260_max_str_len);
               if (res < 0)
               {
                  printf("Unable to read manufacturer string\n");
               }
               printf("Manufacturer String: %ls\n", wstr);

               // Read the Product String
               wstr[0] = 0x0000;
               res = hid_get_product_string(handles[i], wstr, hph_ft260_max_str_len);
               if (res < 0)
               {
                  printf("Unable to read product string\n");
               }
               printf("Product String: %ls\n", wstr);

               // Read the Serial Number String
               wstr[0] = 0x0000;
               res = hid_get_serial_number_string(handles[i], wstr, hph_ft260_max_str_len);
               if (res < 0)
               {
                  printf("Unable to read serial number string\n");
               }
               printf("Serial Number String: (%d) %ls\n", wstr[0], wstr);

#ifdef HPH_FT260_INTERFACE_DEBUG
               print_hid_report_descriptor_from_device(handles[i]);


               struct hid_device_info* info = hid_get_device_info(handles[i]);
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
               res = hid_get_indexed_string(handles[i], 1, wstr, hph_ft260_max_str_len);
               if (res < 0)
               {
                  printf("Unable to read indexed string 1\n");
               }
               printf("Indexed String 1: %ls\n", wstr);
#endif
               printf("\n");

               //clear the device buffers
               if(set_as_non_blocking(i) < 0)
               {
                  (*error_code)[i] = 3; // hid_device unable to change blocking state error
               }

               if(read_data(i) < 0)
               {
                  (*error_code)[i] = 4; // hid_device unable to read data error
               }

               if(set_as_blocking(i) < 0)
               {
                  (*error_code)[i] = 3; // hid_device unable to change blocking state error
               }
            }
         }

         hid_free_enumeration(devs);
      }

      buffer_slots_used = 0;
   }

   ft260_interface::~ft260_interface(void)
   {
      for(int i = 0; i < devices; ++i)
      {
         hid_close(handles[i]);
      }
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

}
