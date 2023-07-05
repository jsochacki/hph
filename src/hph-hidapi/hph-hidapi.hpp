#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <string> // for wstring

#include <hidapi/hidapi.h>

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

// Fallback/example
#ifndef HID_API_MAKE_VERSION
#define HID_API_MAKE_VERSION(mj, mn, p) (((mj) << 24) | ((mn) << 8) | (p))
#endif
#ifndef HID_API_VERSION
#define HID_API_VERSION HID_API_MAKE_VERSION(HID_API_VERSION_MAJOR, HID_API_VERSION_MINOR, HID_API_VERSION_PATCH)
#endif

//
// Sample using platform-specific headers
#if defined(__APPLE__) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_darwin.h>
#endif

#if defined(_WIN32) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_winapi.h>
#endif

#if defined(USING_HIDAPI_LIBUSB) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_libusb.h>
#endif

namespace hph
{

   struct hid_device_search_parameters
   {
      wchar_t* manufacturer_string;
      wchar_t* product_string;
      ushort vendor_id;
      ushort product_id;
   };

   //Size Definition
//   hph-emulation@hph-emulation:/usr/include/linux$ sudo grep -nir 'struct hid_device' /usr/src/linux-headers-5.15.0-76-generic/
//   /usr/src/linux-headers-5.15.0-76-generic/scripts/mod/devicetable-offsets.h:22:#define SIZE_hid_device_id 24 /* sizeof(struct hid_device_id) */

   const char *const hid_bus_type_name[] = {"Unknown",
                                            "USB",
                                            "Bluetooth",
                                            "I2C",
                                            "SPI",
                                           };

   const char *hid_bus_name(hid_bus_type bus_type);
   void print_device(struct hid_device_info *cur_dev);
   void print_hid_report_descriptor_from_device(hid_device *device);
   void print_hid_report_descriptor_from_path(const char *path);
   void print_devices(struct hid_device_info *cur_dev);
   void print_devices_with_descriptor(struct hid_device_info *cur_dev);
   int count_devices(struct hid_device_info *cur_dev);
   int get_devices(struct hid_device_info *cur_dev,
                    hid_device_search_parameters dev_to_find,
                    char ** devices_found);
   wchar_t *utf8_to_wchar_t(const char *utf8);
}
