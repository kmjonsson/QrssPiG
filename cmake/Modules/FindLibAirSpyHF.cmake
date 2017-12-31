# Try to find libairspyhf
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LibAirSpyHF_INCLUDE_DIR NAMES libairspyhf/airspyhf.h)

# Search first where lib is installed from source, then system one
find_library(LibAirSpyHF_LIBRARY NAMES libairspyhf airspyhf PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LibAirSpyHF_LIBRARY NAMES libairspyhf airspyhf)

libfind_process(LibAirSpyHF)
