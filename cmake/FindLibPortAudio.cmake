# - Try to find Portaudio
# Once done this will define
#
#  PORTAUDIO_FOUND - system has Portaudio
#  PORTAUDIO_INCLUDE_DIRS - the Portaudio include directory
#  PORTAUDIO_LIBRARIES - Link these to use Portaudio
#  PORTAUDIO_DEFINITIONS - Compiler switches required for using Portaudio
#  PORTAUDIO_VERSION - Portaudio version
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  # in cache already
  set(PORTAUDIO_FOUND TRUE)
else (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  if (NOT WIN32)
    include(FindPkgConfig)
    pkg_check_modules(PORTAUDIO2 portaudio-2.0)
  endif (NOT WIN32)

  find_path (PORTAUDIO_INCLUDE_DIRS
    NAMES
      portaudio.h
      PATHS
        ${PORTAUDIO2_INCLUDEDIR}
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
    )
  if (NOT PORTAUDIO_INCLUDE_DIRS)
    set (PORTAUDIO_INCLUDE_DIRS
      ${PORTAUDIO2_INCLUDE_DIRS}
    )
    list (APPEND PORTAUDIO_INCLUDE_DIRS ${PORTAUDIO2_INCLUDEDIR})
  endif (NOT PORTAUDIO_INCLUDE_DIRS)

  find_library (PORTAUDIO_LIBRARIES
    NAMES
      portaudio
    PATHS
      ${PORTAUDIO2_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )
  if (NOT PORTAUDIO_LIBRARIES)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      set(PORTAUDIO_LIBRARIES "${PORTAUDIO2_LIBDIR}/lib${PORTAUDIO2_LIBRARIES}.dylib")
    else (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      set(PORTAUDIO_LIBRARIES
        ${PORTAUDIO2_LIBRARIES}
    )
    endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  endif (NOT PORTAUDIO_LIBRARIES)

  if (PORTAUDIO2_VERSION)
    set(PORTAUDIO_VERSION
      ${PORTAUDIO2_VERSION}
    )
  else (PORTAUDIO2_VERSION)
    set(PORTAUDIO_VERSION
      18
    )
  endif (PORTAUDIO2_VERSION)

  if (PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)
     set(PORTAUDIO_FOUND TRUE)
  endif (PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)
 
  if (PORTAUDIO_FOUND)
    if (NOT Portaudio_FIND_QUIETLY)
      message(STATUS "Found Portaudio: ${PORTAUDIO_LIBRARIES}")
    endif (NOT Portaudio_FIND_QUIETLY)
  else (PORTAUDIO_FOUND)
    if (Portaudio_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Portaudio")
    endif (Portaudio_FIND_REQUIRED)
  endif (PORTAUDIO_FOUND)

  # show the PORTAUDIO_INCLUDE_DIRS and PORTAUDIO_LIBRARIES variables only in the advanced view
  mark_as_advanced(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)

endif (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
