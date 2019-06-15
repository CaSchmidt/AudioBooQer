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

add_subdirectory(3rdparty/fdk-aac)
add_subdirectory(3rdparty/mp4v2)

### AudioBooQer ##############################################################

set(AudioBooQer_FORMS
  forms/waudioformat.ui
  forms/waudioplayer.ui
  forms/wjobinfo.ui
  forms/wmainwindow.ui
  )

set(AudioBooQer_HEADERS
  include/aacencoder.h
  include/audiojob.h
  include/chapter.h
  include/chaptermodel.h
  include/iaudioencoder.h
  include/job.h
  include/mpeg4_asc.h
  include/output.h
  include/rawencoder.h
  include/waudioformat.h
  include/waudioplayer.h
  include/wjobinfo.h
  include/wmainwindow.h
  )

set(AudioBooQer_SOURCES
  src/aacencoder.cpp
  src/audiojob.cpp
  src/chapter.cpp
  src/chaptermodel.cpp
  src/iaudioencoder.cpp
  src/job.cpp
  src/main.cpp
  src/output.cpp
  src/rawencoder.cpp
  src/waudioformat.cpp
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

target_link_libraries(AudioBooQer fdk-aac mp4v2 csQt Qt5::Widgets Qt5::Multimedia Qt5::Concurrent)

format_output_name(AudioBooQer "AudioBooQer")

target_compile_definitions(AudioBooQer PRIVATE
  MP4V2_USE_STATIC_LIB
  )

target_include_directories(AudioBooQer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(AudioBooQer PRIVATE ${csQt_SOURCE_DIR}/include)
target_include_directories(AudioBooQer PRIVATE ${csQt_BINARY_DIR}/include)
target_include_directories(AudioBooQer PRIVATE
  3rdparty/fdk-aac/fdk-aac-src/libAACenc/include
  3rdparty/fdk-aac/fdk-aac-src/libSYS/include
  3rdparty/mp4v2/mp4v2-src/include
  )

install(TARGETS AudioBooQer
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )
