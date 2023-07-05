#include "hph.hpp"

int main(int argc, char* argv[])
{
   int *error_code = (int*) calloc(1, sizeof(int));

   hph::ft260_interface ft260s(error_code);

   /*

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
