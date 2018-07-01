# - Try to find Libsndfile
# Once done this will define
#
#  LIBSNDFILE_FOUND - System has LIBSNDFILE
#  LIBSNDFILE_INCLUDE_DIR - The SNDFILE include directory
#  LIBSNDFILE_LIBRARY - The library needed to use SNDFILE
#

find_path(LIBSNDFILE_INCLUDE_DIR sndfile.h)

SET(LIBSNDFILE_NAMES ${LIBSNDFILE_NAMES} sndfile libsndfile)
FIND_LIBRARY(LIBSNDFILE_LIBRARY NAMES ${LIBSNDFILE_NAMES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBSNDFILE DEFAULT_MSG LIBSNDFILE_LIBRARY
                                    LIBSNDFILE_INCLUDE_DIR)