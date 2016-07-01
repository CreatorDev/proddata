SET(GMOCK_DIR "" CACHE PATH "Path to Gmock directory")

#Dependencies
##############
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(GTEST gtest>=1.7.0)
PKG_CHECK_MODULES(GMOCK gmock>=1.7.0)

# Paths
########
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/src ${CXXTEST_INCLUDE_DIR})
IF(GTEST_FOUND AND GMOCK_FOUND)
  INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
ELSE()
  INCLUDE_DIRECTORIES(${GMOCK_DIR} ${GMOCK_DIR}/include ${GMOCK_DIR}/gtest/include ${GMOCK_DIR}/gtest)
ENDIF()

# Build Gmock
#############
IF(GTEST_FOUND AND GMOCK_FOUND)
  SET(UTEST_FRAMEWORK_LIBS ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})
ELSE()
IF(CHECK_DEP)
  ADD_LIBRARY(gmock-static STATIC ${GMOCK_DIR}/gtest/src/gtest-all.cc ${GMOCK_DIR}/src/gmock-all.cc)
  SET(UTEST_FRAMEWORK_LIBS gmock-static)
ENDIF()
ENDIF()

# Add unittest targets
########################
CXXTEST_ADD_TEST(utest_device_data test_deivce_data.cc ${CMAKE_CURRENT_SOURCE_DIR}/test_device_data.h
                 ${CMAKE_SOURCE_DIR}/src/device_data.cc ${CMAKE_SOURCE_DIR}/src/vector_operations.cc
                 ${CMAKE_SOURCE_DIR}/src/flash_access.cc)
TARGET_LINK_LIBRARIES(utest_device_data crclib pthread ${GLOG_LIBRARIES} ${UTEST_FRAMEWORK_LIBS})

# Add valgrind targets
######################
VALGRIND_ADD_TEST(utest_device_data)

# Add cpplint target
######################
file(GLOB CPPLINT_CHECK *.cc *.h)
CPPLINT_ADD(utest ${CPPLINT_CHECK})
