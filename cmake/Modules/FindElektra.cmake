find_package(PkgConfig)
pkg_check_modules(ELEKTRA elektra)


if(ELEKTRA_CFLAGS AND ELEKTRA_LIBRARY_DIRS)

  # query pkg-config asking for Elektra >= 0.7.0
  EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=0.7.0 elektra RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )

  if(_return_VALUE STREQUAL "0")
    set(ELEKTRA_FOUND TRUE)
    set(HAVE_ELEKTRA TRUE)
  else(_return_VALUE STREQUAL "0")
    message(STATUS "Elektra >= 0.7.0 was found")
  endif(_return_VALUE STREQUAL "0")
endif(ELEKTRA_CFLAGS AND ELEKTRA_LIBRARY_DIRS)

if (ELEKTRA_FOUND)
    if (NOT Elektra_FIND_QUIETLY)
        message(STATUS "Found ELEKTRA: ${ELEKTRA_LIBRARY_DIRS} ${ELEKTRA_INCLUDE_DIRS}")
    endif (NOT Elektra_FIND_QUIETLY)
else (ELEKTRA_FOUND)
    if (NOT Elektra_FIND_QUIETLY)
        message(STATUS "Elektra was NOT found.")
    endif (NOT Elektra_FIND_QUIETLY)
    if (Elektra_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find Elektra")
    endif (Elektra_FIND_REQUIRED)
endif (ELEKTRA_FOUND)
