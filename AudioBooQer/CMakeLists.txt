cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

### Project ##################################################################

project(AudioBooQer)

### Modules ##################################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FormatOutputName)

### Dependencies #############################################################

find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Multimedia 5.6 REQUIRED)
find_package(Qt5Concurrent 5.6 REQUIRED)

add_subdirectory(../csQt/csQt
  ${CMAKE_CURRENT_BINARY_DIR}/csQt
  )

set(ENABLE_QT ON CACHE BOOL "" FORCE)

add_subdirectory(../csUtil/csUtil
  ${CMAKE_CURRENT_BINARY_DIR}/csUtil
  )

add_subdirectory(${AudioBooQer_SOURCE_DIR}/3rdparty/miniaudio
  ${CMAKE_CURRENT_BINARY_DIR}/miniaudio
  )

### Project ##################################################################

add_subdirectory(audiobook)
add_subdirectory(ui)
