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

add_subdirectory(../csUtil/csUtil
  ${CMAKE_CURRENT_BINARY_DIR}/csUtil
  )

add_subdirectory(audiobook)

### AudioBooQer ##############################################################

set(AudioBooQer_RESOURCES
  resources/resources.qrc
  )

set(AudioBooQer_FORMS
  forms/waudioformat.ui
  forms/waudioplayer.ui
  forms/WBookBinder.ui
  forms/wjobinfo.ui
  forms/wmainwindow.ui
  forms/WTagEditor.ui
  )

set(AudioBooQer_HEADERS
  include/audiojob.h
  include/BookBinderModel.h
  include/chapter.h
  include/chaptermodel.h
  include/job.h
  include/waudioformat.h
  include/waudioplayer.h
  include/WBookBinder.h
  include/wjobinfo.h
  include/wmainwindow.h
  include/WTagEditor.h
  )

set(AudioBooQer_SOURCES
  src/audiojob.cpp
  src/BookBinderModel.cpp
  src/chapter.cpp
  src/chaptermodel.cpp
  src/job.cpp
  src/main.cpp
  src/waudioformat.cpp
  src/waudioplayer.cpp
  src/WBookBinder.cpp
  src/wjobinfo.cpp
  src/wmainwindow.cpp
  src/WTagEditor.cpp
  )

### Target ###################################################################

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

add_executable(AudioBooQer
  ${AudioBooQer_RESOURCES}
  ${AudioBooQer_FORMS}
  ${AudioBooQer_HEADERS}
  ${AudioBooQer_SOURCES}
  )

if(WIN32)
  set_target_properties(AudioBooQer PROPERTIES WIN32_EXECUTABLE ON)
endif()

target_link_libraries(AudioBooQer audiobook csQt Qt5::Widgets Qt5::Multimedia Qt5::Concurrent)

format_output_name(AudioBooQer "AudioBooQer")

target_include_directories(AudioBooQer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(AudioBooQer PRIVATE ${csQt_SOURCE_DIR}/include)
target_include_directories(AudioBooQer PRIVATE ${csQt_BINARY_DIR}/include)

set_target_properties(AudioBooQer PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
  AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms
  )

install(TARGETS AudioBooQer
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )
