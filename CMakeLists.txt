# Project Properties
####################
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.8)
PROJECT(proddata)
# This will be extended to take the version from SCM
# Version config can be simplified from 3.0+
SET(proddata_VERSION_MAJOR 0)
SET(proddata_VERSION_MINOR 1)
SET(proddata_VERSION_PATCH 0)
SET(proddata_VERSION
    ${proddata_VERSION_MAJOR}.${proddata_VERSION_MINOR}.${proddata_VERSION_PATCH})

# Options
###################
SET(CMAKE_VERBOSE_MAKEFILE 1)
SET(CMAKE_BUILD_TYPE DEBUG) # Options MINSIZEREL, RELEASE, DEBUG
ADD_COMPILE_OPTIONS(-Wall -Werror)
SET(CMAKE_CXX_FLAGS "-std=gnu++11")
OPTION(BUILD_TESTS "Add test target" ON)

# Includes
##########
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
  IF(DEFINED ENV{CASKEID_COMMON})
    LIST(APPEND CMAKE_MODULE_PATH "$ENV{CASKEID_COMMON}/cmake")
    INCLUDE(util)
    COPY_FILES(cmake "$ENV{CASKEID_COMMON}/cmake" ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)
  ELSE()
    MESSAGE(WARNING "Environment variable CASKEID_COMMON is not set; \nexport CASKEID_COMMON=<common path>")
  ENDIF()
ENDIF()

INCLUDE(check_dep)
INCLUDE(cpplint)

# Paths
########
ADD_SUBDIRECTORY(inc)
ADD_SUBDIRECTORY(src)
IF(BUILD_TESTS)
  FIND_PACKAGE(CxxTest ${STRICT_CHECK})
  IF(CXXTEST_FOUND)
    INCLUDE(memcheck)
    INCLUDE(cxxtest_objlib)
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(utest)
  ENDIF()
ENDIF()
ADD_SUBDIRECTORY(docs)

