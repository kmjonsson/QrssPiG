# Try to find libhackrf
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LibHackRF_INCLUDE_DIR NAMES libhackrf/hackrf.h)

# Search first where lib is installed from source, then system one
find_library(LibHackRF_LIBRARY NAMES libhackrf hackrf PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LibHackRF_LIBRARY NAMES libhackrf hackrf)

libfind_process(LibHackRF)

include(CheckSymbolExists)

set(CMAKE_REQUIRED_INCLUDES ${LibHackRF_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${LibHackRF_LIBRARY})

CHECK_SYMBOL_EXISTS(hackrf_library_version libhackrf/hackrf.h LibHackRF_HAS_hackrf_library_version)
