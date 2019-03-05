# Install script for directory: /home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils

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
   "/home/naiqi/AL2018/include/AL/usdmaya/utils/Api.h;/home/naiqi/AL2018/include/AL/usdmaya/utils/AttributeType.h;/home/naiqi/AL2018/include/AL/usdmaya/utils/DgNodeHelper.h;/home/naiqi/AL2018/include/AL/usdmaya/utils/Utils.h;/home/naiqi/AL2018/include/AL/usdmaya/utils/ForwardDeclares.h;/home/naiqi/AL2018/include/AL/usdmaya/utils/MeshUtils.h;/home/naiqi/AL2018/include/AL/usdmaya/utils/NurbsCurveUtils.h;/home/naiqi/AL2018/include/AL/usdmaya/utils/DiffPrimVar.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/usdmaya/utils" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils/Api.h"
    "/home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils/AttributeType.h"
    "/home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils/DgNodeHelper.h"
    "/home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils/Utils.h"
    "/home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils/ForwardDeclares.h"
    "/home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils/MeshUtils.h"
    "/home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils/NurbsCurveUtils.h"
    "/home/naiqi/repos/AL_USDMaya/usdmayautils/AL/usdmaya/utils/DiffPrimVar.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaUtils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaUtils.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaUtils.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/libAL_USDMayaUtils.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib" TYPE SHARED_LIBRARY FILES "/home/naiqi/repos/AL_USDMaya/2018build/usdmayautils/AL/usdmaya/utils/libAL_USDMayaUtils.so")
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaUtils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaUtils.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaUtils.so"
         OLD_RPATH "/home/naiqi/repos/AL_USDMaya/2018build/usdutils/AL/usd/utils:/home/naiqi/repos/AL_USDMaya/2018build/mayautils/AL/maya:/usr/local/lib64:/usr/local/USD/lib:/usr/autodesk/maya2018/lib:/home/naiqi/repos/AL_USDMaya/2018build/utils/AL:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMayaUtils.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

