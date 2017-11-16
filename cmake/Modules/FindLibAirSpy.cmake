# Try to find libairspy
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LIBAIRSPY_INCLUDE_DIR NAMES libairspy/airspy.h)

# Search first where lib is installed from source, then system one
find_library(LIBAIRSPY_LIBRARY NAMES libairspy airspy PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LIBAIRSPY_LIBRARY NAMES libairspy airspy)

libfind_process(LIBAIRSPY)
