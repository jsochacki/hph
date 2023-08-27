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

   int count_devices(struct hid_device_info *cur_dev)
   {
      int device_count = 0;
      for (; cur_dev; cur_dev = cur_dev->next)
      {
         ++device_count;
      }
      return device_count;
   }

   int get_devices(struct hid_device_info *cur_dev, hid_device_search_parameters dev_to_find, std::vector<std::string> &devices_found, std::vector<int> &corresponding_interface_numbers)
   {
      int found_device_count = 0;
      std::wstring tempstr2(dev_to_find.manufacturer_string);
      std::wstring tempstr4(dev_to_find.product_string);
      for(; cur_dev; cur_dev = cur_dev->next)
      {
         if((cur_dev->manufacturer_string != NULL) && (cur_dev->product_string != NULL))
         {
            std::wstring tempstr1(cur_dev->manufacturer_string);
            std::wstring tempstr3(cur_dev->product_string);
            if((tempstr1 == tempstr2)
               && (tempstr3 == tempstr4)
               && (cur_dev->vendor_id == dev_to_find.vendor_id)
               && (cur_dev->product_id == dev_to_find.product_id))
               {
                  //hid.c uses strdup to set path member
                  //strdup which Returns a pointer to a null-terminated byte string
                  //so we can use strlen for length of char array
                  devices_found.emplace_back(cur_dev->path);
                  corresponding_interface_numbers.push_back(cur_dev->interface_number);
                  ++found_device_count;
               }
         }
      }

      return found_device_count;
   }

   wchar_t *utf8_to_wchar_t(const char *utf8)
   {
      wchar_t *ret = NULL;

      if (utf8) {
         size_t wlen = mbstowcs(NULL, utf8, 0);
         if ((size_t) -1 == wlen) {
            return wcsdup(L"");
         }
         ret = (wchar_t*) calloc(wlen+1, sizeof(wchar_t));
         if (ret == NULL) {
            /* as much as we can do at this point */
            return NULL;
         }
         mbstowcs(ret, utf8, wlen+1);
         ret[wlen] = 0x0000;
      }

      return ret;
   }
}
