# Try to find libosmosdr
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LIBOSMOSDR_INCLUDE_DIR NAMES osmosdr.h)

# Search first where lib is installed from source, then system one
find_library(LIBOSMOSDR_LIBRARY NAMES libosmosdr osmosdr PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LIBOSMOSDR_LIBRARY NAMES libosmosdr osmosdr)

libfind_process(LIBOSMOSDR)
