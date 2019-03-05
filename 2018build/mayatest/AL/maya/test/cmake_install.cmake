# Install script for directory: /home/naiqi/repos/AL_USDMaya/mayatest/AL/maya/test

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/naiqi/AL2018")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/include/AL/maya/test/Api.h;/home/naiqi/AL2018/include/AL/maya/test/testHelpers.h;/home/naiqi/AL2018/include/AL/maya/test/testHarness.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/maya/test" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/mayatest/AL/maya/test/Api.h"
    "/home/naiqi/repos/AL_USDMaya/mayatest/AL/maya/test/testHelpers.h"
    "/home/naiqi/repos/AL_USDMaya/mayatest/AL/maya/test/testHarness.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_MayaTest.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_MayaTest.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_MayaTest.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/libAL_MayaTest.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib" TYPE SHARED_LIBRARY FILES "/home/naiqi/repos/AL_USDMaya/2018build/mayatest/AL/maya/test/libAL_MayaTest.so")
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_MayaTest.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_MayaTest.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_MayaTest.so"
         OLD_RPATH "/usr/local/USD/lib:/usr/autodesk/maya2018/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_MayaTest.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

