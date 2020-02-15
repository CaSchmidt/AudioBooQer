### Dependencies #############################################################

add_subdirectory(${AudioBooQer_SOURCE_DIR}/3rdparty/fdk-aac
  ${CMAKE_CURRENT_BINARY_DIR}/fdk-aac
  )
add_subdirectory(${AudioBooQer_SOURCE_DIR}/3rdparty/mp4v2
  ${CMAKE_CURRENT_BINARY_DIR}/mp4v2
  )

### Project ##################################################################

list(APPEND audiobook_HEADERS
  include/AacFormat.h
  include/Mp4Tag.h
  )

list(APPEND audiobook_SOURCES
  src/AacFormat.cpp
  src/Mp4Tag.cpp
  )

### Target ###################################################################

add_library(audiobook STATIC
  ${audiobook_HEADERS}
  ${audiobook_SOURCES}
  )

format_output_name(audiobook "audiobook")

target_include_directories(audiobook
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(audiobook
  PUBLIC fdk-aac mp4v2
  PUBLIC  csUtil
  )
