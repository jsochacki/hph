#include "hph-ft260.hpp"

int main(int argc, char* argv[])
{
   int res;
   unsigned char buf[HPH_FT260_MAX_CHAR_BUF];
   wchar_t wstr[HPH_FT260_MAX_STR];
   hid_device *handle;
   int i;

   // Initialize the hidapi library
   res = hid_init();

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

   return 0;
}
