#include "hph-ft260.hpp"

namespace hph
{
   ft260_interface::ft260_interface(int *error_code_out)
      : error_code(error_code_out)
   {
      char manufacturer_string[] = "FTDI";
      char product_string[] = "FT260";
      ft260_device_parameters.manufacturer_string = utf8_to_wchar_t(manufacturer_string);
      ft260_device_parameters.product_string = utf8_to_wchar_t(product_string);
      ft260_device_parameters.vendor_id = HPH_FT260_VENDOR_ID;
      ft260_device_parameters.product_id = HPH_FT260_PRODUCT_ID;

      /*
      printf("mfg string is %ls\n", ft260_device_parameters.manufacturer_string);
      printf("product string is %ls\n", ft260_device_parameters.product_string);
      printf("vendor id is %hu\n", ft260_device_parameters.vendor_id);
      printf("product id is %hu\n", ft260_device_parameters.product_id);
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
         *error_code = 1; // hid_init failure error
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
         //print_devices_with_descriptor(devs);

         printf("total hid devices found is %d\n", total_devices);

         char **devices_found = (char**) calloc(total_devices, sizeof(char*));
         ft260_devices = get_devices(devs, ft260_device_parameters, devices_found);

         printf("ft260 hid devices found is %d\n", ft260_devices);

         for(int i = 0; i < ft260_devices; ++i)
         {
            printf("ft260 device %d path is %s\n", i, devices_found[i]);
         }

         hid_free_enumeration(devs);

         /*

         // Open the device using the VID, PID,
         // and optionally the Serial number.
         handle = hid_open(FT260_VENDOR_ID, FT260_PRODUCT_ID, NULL);

         if (!handle)
         {
            printf("Unable to open device\n");
            hid_exit();
            return 1;
         }

         // Read the Manufacturer String
         res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
         printf("Manufacturer String: %ls\n", wstr);

         // Read the Product String
         res = hid_get_product_string(handle, wstr, MAX_STR);
         printf("Product String: %ls\n", wstr);

         // Read the Serial Number String
         res = hid_get_serial_number_string(handle, wstr, MAX_STR);
         printf("Serial Number String: (%d) %ls\n", wstr[0], wstr);

         // Read Indexed String 1
         res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
         printf("Indexed String 1: %ls\n", wstr);

         // Toggle LED (cmd 0x80). The first byte is the report number (0x0).
         buf[0] = 0xA1;
         buf[1] = 0x22;
         buf[2] = 0x64;
         res = hid_write(handle, buf, 3);

         printf("did state write");

         // Request state (cmd 0x81). The first byte is the report number (0x0).
         buf[0] = 0x80;
         buf[1] = 0xFF;
         buf[2] = 0x01;
         buf[3] = 0xFF;
         buf[4] = 0x01;
         res = hid_write(handle, buf, 5);

         // Read requested state
         res = hid_read(handle, buf, 16);

         // Print out the returned buffer.
         for (i = 0; i < 4; i++)
            printf("buf[%d]: %d\n", i, buf[i]);

         // Close the device
         hid_close(handle);

         // Finalize the hidapi library
         res = hid_exit();
         
         */
      }
   }
}
