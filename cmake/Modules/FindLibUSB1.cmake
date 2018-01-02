# Try to find libusb
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LibUSB1_INCLUDE_DIR NAMES libusb-1.0/libusb.h)

# Search first where lib is installed from source, then system one
find_library(LibUSB1_LIBRARY NAMES libusb-1.0 usb-1.0 PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LibUSB1_LIBRARY NAMES libusb-1.0 usb-1.0)

libfind_process(LibUSB1)
