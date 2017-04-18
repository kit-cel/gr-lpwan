INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_LPWAN lpwan)

FIND_PATH(
    LPWAN_INCLUDE_DIRS
    NAMES lpwan/api.h
    HINTS $ENV{LPWAN_DIR}/include
        ${PC_LPWAN_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    LPWAN_LIBRARIES
    NAMES gnuradio-lpwan
    HINTS $ENV{LPWAN_DIR}/lib
        ${PC_LPWAN_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LPWAN DEFAULT_MSG LPWAN_LIBRARIES LPWAN_INCLUDE_DIRS)
MARK_AS_ADVANCED(LPWAN_LIBRARIES LPWAN_INCLUDE_DIRS)

