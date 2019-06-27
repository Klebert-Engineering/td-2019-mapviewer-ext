cmake_minimum_required (VERSION 3.2)
project (mapviewer.ext.bookmarks)

if (EXISTS ${AFW_SDK_DIR})
    add_subdirectory (${AFW_SDK_DIR} ${CMAKE_CURRENT_BINARY_DIR}/afwsdk)
else ()
    message (FATAL_ERROR "AFW_SDK_DIR (${AFW_SDK_DIR}) missing, please set it.")
endif ()

find_package (Qt5 COMPONENTS Core Concurrent Sql REQUIRED)

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (CXX_STANDARD_REQUIRED ON)

if(WIN32)
  set(CMAKE_SHARED_MODULE_PREFIX "")
endif()

set (NDSAFW_PLUGIN_LINKING "MODULE")

if(WIN32)
  set(CMAKE_SHARED_MODULE_PREFIX "")
  set (CMAKE_DEBUG_POSTFIX d)
elseif (APPLE)
  set (CMAKE_DEBUG_POSTFIX _debug)
  set (NDSAFW_PLUGIN_LINKING "SHARED")
endif()

add_library (${PROJECT_NAME} ${NDSAFW_PLUGIN_LINKING}

  resources.qrc

  src/extension.cpp
  src/extension.h
  src/extensionfactory.cpp
  src/extensionfactory.h
  src/plugin.cpp
  src/plugin.h

  ${PROJECT_NAME}.pluginspec
  )

set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

target_link_libraries (${PROJECT_NAME}
  afw.core
  mapviewer.backend.interface
  extensionsystem
  Qt5::Core
  Qt5::Concurrent
  Qt5::Sql
  )

add_custom_command(TARGET ${PROJECT_NAME} 
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pluginspec "${CMAKE_CURRENT_BINARY_DIR}"
  )
