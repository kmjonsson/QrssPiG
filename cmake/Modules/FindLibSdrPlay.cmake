# Try to find libmirsdrapi
#
# Once successfully done this will define
#  LIB

include(LibFindMacros)

find_path(LibSdrPlay_INCLUDE_DIR NAMES mirsdrapi-rsp.h)

# Search first where lib is installed from source, then system one
find_library(LibSdrPlay_LIBRARY NAMES libmirsdrapi-rsp mirsdrapi-rsp PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
find_library(LibSdrPlay_LIBRARY NAMES libmirsdrapi-rsp mirsdrapi-rsp)

libfind_process(LibSdrPlay)
