# Try to find libgnuradio-osmosdr
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LibGROsmoSdrINCLUDE_DIR NAMES osmosdr/device.h)

# Search first where lib is installed from source, then system one
find_library(LibGROsmoSdr_LIBRARY NAMES libgnuradio-osmosdr gnuradio-osmosdr PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LibGROsmoSdr_LIBRARY NAMES libgnuradio-osmosdr gnuradio-osmosdr)

libfind_process(LibGROsmoSdr)
