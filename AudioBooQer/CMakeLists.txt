cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

### Project ##################################################################

project(AudioBooQer)

### Modules ##################################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FormatOutputName)

### Dependencies #############################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)
find_package(Qt5Widgets 5.6 REQUIRED)
find_package(Qt5Multimedia 5.6 REQUIRED)
find_package(Qt5Concurrent 5.6 REQUIRED)

add_subdirectory(../csQt/csQt
  ${CMAKE_CURRENT_BINARY_DIR}/csQt
  )

### AudioBooQer ##############################################################

set(AudioBooQer_FORMS
  forms/waudioplayer.ui
  forms/wjobinfo.ui
  forms/wmainwindow.ui
  )

set(AudioBooQer_HEADERS
  include/chapter.h
  include/chaptermodel.h
  include/job.h
  include/waudioplayer.h
  include/wjobinfo.h
  include/wmainwindow.h
  )

set(AudioBooQer_SOURCES
  src/chapter.cpp
  src/chaptermodel.cpp
  src/job.cpp
  src/main.cpp
  src/waudioplayer.cpp
  src/wjobinfo.cpp
  src/wmainwindow.cpp
  )

### Target ###################################################################

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

add_executable(AudioBooQer
  ${AudioBooQer_FORMS}
  ${AudioBooQer_HEADERS}
  ${AudioBooQer_SOURCES}
  )

if(WIN32)
  set_target_properties(AudioBooQer PROPERTIES WIN32_EXECUTABLE ON)
endif()

target_link_libraries(AudioBooQer csQt Qt5::Widgets Qt5::Multimedia Qt5::Concurrent)

format_output_name(AudioBooQer "AudioBooQer")

target_include_directories(AudioBooQer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(AudioBooQer PRIVATE ${csQt_SOURCE_DIR}/include)
target_include_directories(AudioBooQer PRIVATE ${csQt_BINARY_DIR}/include)

install(TARGETS AudioBooQer
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )
