# Install script for directory: /home/naiqi/repos/AL_USDMaya/schemas/AL/usd/schemas/maya

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
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaSchemas.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaSchemas.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaSchemas.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/libAL_USDMayaSchemas.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib" TYPE SHARED_LIBRARY FILES "/home/naiqi/repos/AL_USDMaya/2018build/schemas/AL/usd/schemas/maya/libAL_USDMayaSchemas.so")
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaSchemas.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaSchemas.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaSchemas.so"
         OLD_RPATH "/usr/local/USD/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaSchemas.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya/_AL_USDMayaSchemas.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya/_AL_USDMayaSchemas.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya/_AL_USDMayaSchemas.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya/_AL_USDMayaSchemas.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya" TYPE SHARED_LIBRARY FILES "/home/naiqi/repos/AL_USDMaya/2018build/schemas/AL/usd/schemas/maya/_AL_USDMayaSchemas.so")
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya/_AL_USDMayaSchemas.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya/_AL_USDMayaSchemas.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya/_AL_USDMayaSchemas.so"
         OLD_RPATH "/home/naiqi/repos/AL_USDMaya/2018build/schemas/AL/usd/schemas/maya:/usr/local/USD/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usd/schemas/maya/_AL_USDMayaSchemas.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND python -m compileall /home/naiqi/repos/AL_USDMaya/2018build/AL/usd/schemas/maya/__init__.py )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/python/AL")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib/python" TYPE DIRECTORY FILES "/home/naiqi/repos/AL_USDMaya/2018build/AL")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  message(STATUS "POST INSTALL: Compiling python/pyc for /home/naiqi/AL2018/lib/python ... ")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND python -m compileall /home/naiqi/AL2018/lib/python )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/include/AL/usd/schemas/maya/ModelAPI.h;/home/naiqi/AL2018/include/AL/usd/schemas/maya/MayaReference.h;/home/naiqi/AL2018/include/AL/usd/schemas/maya/FrameRange.h;/home/naiqi/AL2018/include/AL/usd/schemas/maya/api.h;/home/naiqi/AL2018/include/AL/usd/schemas/maya/tokens.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/usd/schemas/maya" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/schemas/AL/usd/schemas/maya/ModelAPI.h"
    "/home/naiqi/repos/AL_USDMaya/schemas/AL/usd/schemas/maya/MayaReference.h"
    "/home/naiqi/repos/AL_USDMaya/schemas/AL/usd/schemas/maya/FrameRange.h"
    "/home/naiqi/repos/AL_USDMaya/schemas/AL/usd/schemas/maya/api.h"
    "/home/naiqi/repos/AL_USDMaya/schemas/AL/usd/schemas/maya/tokens.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/usd/AL_USDMayaSchemas/resources/plugInfo.json")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib/usd/AL_USDMayaSchemas/resources" TYPE FILE FILES "/home/naiqi/repos/AL_USDMaya/2018build/schemas/AL/usd/schemas/maya/plugInfo.json")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/usd/AL_USDMayaSchemas/resources/generatedSchema.usda")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib/usd/AL_USDMayaSchemas/resources" TYPE FILE FILES "/home/naiqi/repos/AL_USDMaya/2018build/schemas/AL/usd/schemas/maya/generatedSchema.usda")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/naiqi/repos/AL_USDMaya/2018build/schemas/AL/usd/schemas/maya/tests/cmake_install.cmake")

endif()

