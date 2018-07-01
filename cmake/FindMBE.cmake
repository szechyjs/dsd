# - Try to find MBE
# Once done this will define
#
#  MBE_FOUND - System has MBE
#  MBE_INCLUDE_DIR - The MBE include directory
#  MBE_LIBRARY - The library needed to use MBE
#

find_path(MBE_INCLUDE_DIR mbelib.h)

set(MBE_NAMES ${MBE_NAMES} mbe libmbe)
find_library(MBE_LIBRARY NAMES ${MBE_NAMES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MBE DEFAULT_MSG MBE_LIBRARY MBE_INCLUDE_DIR)
