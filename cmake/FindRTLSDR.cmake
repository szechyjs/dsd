# - Try to find rtlsdr - the hardware driver for the realtek chip in the dvb receivers
# Once done this will define
#  RTLSDR_FOUND - System has rtlsdr
#  RTLSDR_LIBRARIES - The rtlsdr libraries
#  RTLSDR_INCLUDE_DIRS - The rtlsdr include directories
#  RTLSDR_LIB_DIRS - The rtlsdr library directories

if (NOT RTLSDR_FOUND)

  find_package(PkgConfig)
  pkg_check_modules(RTLSDR_PKG librtlsdr)
  set(RTLSDR_DEFINITIONS ${PC_RTLSDR_CFLAGS_OTHER})

  find_path(RTLSDR_INCLUDE_DIR
    NAMES rtl-sdr.h
    HINTS ${RTLSDR_PKG_INCLUDE_DIRS})

  find_library(RTLSDR_LIBRARY
    NAMES rtlsdr
    HINTS ${RTLSDR_PKG_LIBRARY_DIRS})

  set(RTLSDR_LIBRARIES ${RTLSDR_LIBRARY})
  set(RTLSDR_INCLUDE_DIRS ${RTLSDR_INCLUDE_DIR})

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(RTLSDR DEFAULT_MSG RTLSDR_LIBRARY RTLSDR_INCLUDE_DIR)
  mark_as_advanced(RTLSDR_INCLUDE_DIR RTLSDR_LIBRARY)

endif (NOT RTLSDR_FOUND)
