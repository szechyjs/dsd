# - Try to find ITPP
# Once done this will define
#
#  ITPP_FOUND - System has ITPP
#  ITPP_INCLUDE_DIR - The ITPP include directory
#  ITPP_LIBRARY - The library needed to use ITPP
#

find_path(ITPP_INCLUDE_DIR itpp/itcomm.h)

set(ITPP_NAMES ${ITPP_NAMES} itpp libitpp libitpp.dll)
FIND_LIBRARY(ITPP_LIBRARY NAMES ${ITPP_NAMES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ITPP DEFAULT_MSG ITPP_LIBRARY ITPP_INCLUDE_DIR)