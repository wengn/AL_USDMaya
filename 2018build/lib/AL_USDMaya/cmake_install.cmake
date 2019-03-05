# Install script for directory: /home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya

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
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMaya.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMaya.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMaya.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/libAL_USDMaya.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib" TYPE SHARED_LIBRARY FILES "/home/naiqi/repos/AL_USDMaya/2018build/lib/AL_USDMaya/libAL_USDMaya.so")
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMaya.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMaya.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMaya.so"
         OLD_RPATH "/home/naiqi/repos/AL_USDMaya/2018build/usdmayautils/AL/usdmaya/utils:/home/naiqi/repos/AL_USDMaya/2018build/mayautils/AL/maya:/home/naiqi/repos/AL_USDMaya/2018build/usdutils/AL/usd/utils:/usr/local/lib64:/usr/local/USD/lib:/usr/autodesk/maya2018/lib:/home/naiqi/repos/AL_USDMaya/2018build/utils/AL:/usr/local/USD/lib64:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/naiqi/AL2018/lib/libAL_USDMaya.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib/python/AL/usdmaya" TYPE SHARED_LIBRARY FILES "/home/naiqi/repos/AL_USDMaya/2018build/lib/AL_USDMaya/_AL_USDMaya.so")
  if(EXISTS "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so"
         OLD_RPATH "/home/naiqi/repos/AL_USDMaya/2018build/lib/AL_USDMaya:/home/naiqi/repos/AL_USDMaya/2018build/usdmayautils/AL/usdmaya/utils:/home/naiqi/repos/AL_USDMaya/2018build/mayautils/AL/maya:/home/naiqi/repos/AL_USDMaya/2018build/utils/AL:/home/naiqi/repos/AL_USDMaya/2018build/usdutils/AL/usd/utils:/usr/local/lib64:/usr/local/USD/lib:/usr/local/USD/lib64:/usr/autodesk/maya2018/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/naiqi/AL2018/lib/python/AL/usdmaya/_AL_USDMaya.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/python/AL/usdmaya/__init__.py")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib/python/AL/usdmaya" TYPE FILE FILES "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/__init__.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(WRITE "/home/naiqi/AL2018/lib/python/AL/__init__.py"
    "try:
	__import__('pkg_resources').declare_namespace(__name__)
except:
	from pkgutil import extend_path
	__path__ = extend_path(__path__, __name__)")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/lib/usd/AL_USDMaya/resources/plugInfo.json")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/lib/usd/AL_USDMaya/resources" TYPE FILE FILES "/home/naiqi/repos/AL_USDMaya/2018build/lib/AL_USDMaya/plugInfo.json")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(WRITE "/home/naiqi/AL2018/lib/usd/plugInfo.json" "{
    \"Includes\": [ \"*/resources/\" ]
}")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/include/AL/usdmaya/Api.h;/home/naiqi/AL2018/include/AL/usdmaya/DebugCodes.h;/home/naiqi/AL2018/include/AL/usdmaya/DrivenTransformsData.h;/home/naiqi/AL2018/include/AL/usdmaya/Metadata.h;/home/naiqi/AL2018/include/AL/usdmaya/PluginRegister.h;/home/naiqi/AL2018/include/AL/usdmaya/SelectabilityDB.h;/home/naiqi/AL2018/include/AL/usdmaya/StageCache.h;/home/naiqi/AL2018/include/AL/usdmaya/StageData.h;/home/naiqi/AL2018/include/AL/usdmaya/TransformOperation.h;/home/naiqi/AL2018/include/AL/usdmaya/TypeIDs.h;/home/naiqi/AL2018/include/AL/usdmaya/ForwardDeclares.h;/home/naiqi/AL2018/include/AL/usdmaya/Global.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/usdmaya" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/Api.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/DebugCodes.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/DrivenTransformsData.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/Metadata.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/PluginRegister.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/SelectabilityDB.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/StageCache.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/StageData.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/TransformOperation.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/TypeIDs.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/ForwardDeclares.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/Global.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/include/AL/usdmaya/cmds/CreateUsdPrim.h;/home/naiqi/AL2018/include/AL/usdmaya/cmds/DebugCommands.h;/home/naiqi/AL2018/include/AL/usdmaya/cmds/EventCommand.h;/home/naiqi/AL2018/include/AL/usdmaya/cmds/LayerCommands.h;/home/naiqi/AL2018/include/AL/usdmaya/cmds/ProxyShapeCommands.h;/home/naiqi/AL2018/include/AL/usdmaya/cmds/ProxyShapeSelectCommands.h;/home/naiqi/AL2018/include/AL/usdmaya/cmds/ProxyShapePostLoadProcess.h;/home/naiqi/AL2018/include/AL/usdmaya/cmds/RendererCommands.h;/home/naiqi/AL2018/include/AL/usdmaya/cmds/UnloadPrim.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/usdmaya/cmds" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/CreateUsdPrim.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/DebugCommands.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/EventCommand.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/LayerCommands.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/ProxyShapeCommands.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/ProxyShapeSelectCommands.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/ProxyShapePostLoadProcess.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/RendererCommands.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/cmds/UnloadPrim.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/include/AL/usdmaya/fileio/AnimationTranslator.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/Export.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/ExportParams.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/ExportTranslator.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/Import.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/ImportParams.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/ImportTranslator.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/NodeFactory.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/SchemaPrims.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/TransformIterator.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/usdmaya/fileio" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/AnimationTranslator.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/Export.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/ExportParams.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/ExportTranslator.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/Import.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/ImportParams.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/ImportTranslator.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/NodeFactory.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/SchemaPrims.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/TransformIterator.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/include/AL/usdmaya/nodes/Engine.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/Layer.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/LayerManager.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/MeshAnimCreator.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/MeshAnimDeformer.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/ProxyDrawOverride.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/ProxyShape.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/ProxyShapeUI.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/RendererManager.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/Transform.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/TransformationMatrix.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/usdmaya/nodes" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/Engine.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/Layer.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/LayerManager.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/MeshAnimCreator.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/MeshAnimDeformer.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/ProxyDrawOverride.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/ProxyShape.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/ProxyShapeUI.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/RendererManager.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/Transform.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/TransformationMatrix.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/include/AL/usdmaya/nodes/proxy/DrivenTransforms.h;/home/naiqi/AL2018/include/AL/usdmaya/nodes/proxy/PrimFilter.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/usdmaya/nodes/proxy" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/proxy/DrivenTransforms.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/nodes/proxy/PrimFilter.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/DagNodeTranslator.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/DgNodeTranslator.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/TransformTranslator.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/TranslatorBase.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/TranslatorContext.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/TranslatorTestPlugin.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/TranslatorTestType.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/ExtraDataPlugin.h;/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators/TestExtraDataPlugin.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/naiqi/AL2018/include/AL/usdmaya/fileio/translators" TYPE FILE FILES
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/DagNodeTranslator.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/DgNodeTranslator.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/TransformTranslator.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/TranslatorBase.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/TranslatorContext.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/TranslatorTestPlugin.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/TranslatorTestType.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/ExtraDataPlugin.h"
    "/home/naiqi/repos/AL_USDMaya/lib/AL_USDMaya/AL/usdmaya/fileio/translators/TestExtraDataPlugin.h"
    )
endif()

