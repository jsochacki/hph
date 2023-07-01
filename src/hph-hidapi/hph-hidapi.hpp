#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

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

}
