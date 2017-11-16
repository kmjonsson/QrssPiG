# Try to find liblimesuite
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LIBLIMESUITE_INCLUDE_DIR NAMES lime/LimeSuite.h)

# Search first where lib is installed from source, then system one
find_library(LIBLIMESUITE_LIBRARY NAMES libLimeSuite LimeSuite PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LIBLIMESUITE_LIBRARY NAMES libLimeSuite LimeSuite)

libfind_process(LIBLIMESUITE)
