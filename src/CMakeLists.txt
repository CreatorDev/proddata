# Paths
########
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/src)

INCLUDE(CheckLibraryExists)
INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(GLOG ${STRICT_CHECK} libglog)

INCLUDE_DIRECTORIES(${GLOG_INCLUDE_DIRS})

SET(SOURCES main.cc proddata.cc device_data.cc flash_access.cc mtd_access.cc userotp_access.cc vector_operations.cc)
ADD_LIBRARY(crclib SHARED lib_crc.c)

# Add executable targets
########################
ADD_EXECUTABLE(proddata ${SOURCES})
TARGET_LINK_LIBRARIES(proddata crclib ${GLOG_LIBRARIES})

# Add install targets
######################
INSTALL(TARGETS proddata RUNTIME DESTINATION bin)
INSTALL(TARGETS crclib LIBRARY DESTINATION lib)
INSTALL(PROGRAMS wifi_cal.sh DESTINATION bin)

# Add cpplint targets
######################
FILE(GLOB CPPLINT_CHECK *.cc *.h)
LIST(REMOVE_ITEM CPPLINT_CHECK ${CMAKE_CURRENT_SOURCE_DIR}/lib_crc.h)
CPPLINT_ADD(src ${CPPLINT_CHECK})
