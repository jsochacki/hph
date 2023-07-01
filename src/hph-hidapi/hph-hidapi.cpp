#include "hph-hidapi.hpp"

namespace hph
{
   const char *hid_bus_name(hid_bus_type bus_type)
   {
      if ((int)bus_type < 0)
      {
         bus_type = HID_API_BUS_UNKNOWN;
      }

      if ((int)bus_type >= (int)(sizeof(hid_bus_type_name) / sizeof(hid_bus_type_name[0])))
      {
         bus_type = HID_API_BUS_UNKNOWN;
      }

      return hid_bus_type_name[bus_type];
   }

   void print_device(struct hid_device_info *cur_dev)
   {
      printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
      printf("\n");
      printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
      printf("  Product:      %ls\n", cur_dev->product_string);
      printf("  Release:      %hx\n", cur_dev->release_number);
      printf("  Interface:    %d\n",  cur_dev->interface_number);
      printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
      printf("  Bus type: %d (%s)\n", cur_dev->bus_type, hid_bus_name(cur_dev->bus_type));
      printf("\n");
   }

   void print_hid_report_descriptor_from_device(hid_device *device)
   {
      unsigned char descriptor[HID_API_MAX_REPORT_DESCRIPTOR_SIZE];
      int res = 0;

      printf("  Report Descriptor: ");
      res = hid_get_report_descriptor(device, descriptor, sizeof(descriptor));

      if (res < 0)
      {
         printf("error getting: %ls", hid_error(device));
      }
      else
      {
         printf("(%d bytes)", res);
      }
      for (int i = 0; i < res; i++)
      {
         if (i % 10 == 0)
         {
            printf("\n");
         }
         printf("0x%02x, ", descriptor[i]);
      }
      printf("\n");
   }

   void print_hid_report_descriptor_from_path(const char *path)
   {
      hid_device *device = hid_open_path(path);
      if (device)
      {
         print_hid_report_descriptor_from_device(device);
         hid_close(device);
      }
      else
      {
         printf("  Report Descriptor: Unable to open device by path\n");
      }
   }

   void print_devices(struct hid_device_info *cur_dev)
   {
      for (; cur_dev; cur_dev = cur_dev->next)
      {
         print_device(cur_dev);
      }
   }

   void print_devices_with_descriptor(struct hid_device_info *cur_dev)
   {
      for (; cur_dev; cur_dev = cur_dev->next)
      {
         print_device(cur_dev);
         print_hid_report_descriptor_from_path(cur_dev->path);
      }
   }

}
