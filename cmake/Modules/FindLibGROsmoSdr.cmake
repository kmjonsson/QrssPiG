# Try to find libgnuradio-osmosdr
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LIBGROSMOSDR_INCLUDE_DIR NAMES osmosdr/device.h)

# Search first where lib is installed from source, then system one
find_library(LIBGROSMOSDR_LIBRARY NAMES libgnuradio-osmosdr gnuradio-osmosdr PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LIBGROSMOSDR_LIBRARY NAMES libgnuradio-osmosdr gnuradio-osmosdr)

libfind_process(LIBGROSMOSDR)
