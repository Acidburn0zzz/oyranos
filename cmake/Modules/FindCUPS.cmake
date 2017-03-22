# Copyright (c) 2017, Kai-Uwe Behrmann, ku.b@gmx.de
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# use cmake default
FIND_PACKAGE( Cups )

# fix the variable handling
FIND_LIBRARY(CUPS_LIBRARY NAMES cups  ONLY_CMAKE_FIND_ROOT_PATH)

# handle the QUIETLY and REQUIRED arguments and set CUPS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CUPS DEFAULT_MSG CUPS_LIBRARY)

IF(CUPS_FOUND)
  SET( HAVE_CUPS TRUE )
ENDIF(CUPS_FOUND)

MARK_AS_ADVANCED(CUPS_INCLUDE_DIR CUPS_LIBRARIES)
