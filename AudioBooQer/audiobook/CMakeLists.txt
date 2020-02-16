### Dependencies #############################################################

add_subdirectory(${AudioBooQer_SOURCE_DIR}/3rdparty/fdk-aac
  ${CMAKE_CURRENT_BINARY_DIR}/fdk-aac
  )
add_subdirectory(${AudioBooQer_SOURCE_DIR}/3rdparty/mp4v2
  ${CMAKE_CURRENT_BINARY_DIR}/mp4v2
  )

### Project ##################################################################

list(APPEND audiobook_HEADERS
  include/AacEncoder.h
  include/AacFormat.h
  include/AdtsParser.h
  include/BookBinder.h
  include/IAudioEncoder.h
  include/Mp4Tag.h
  include/Mpeg4Audio.h
  include/RawEncoder.h
  )

list(APPEND audiobook_SOURCES
  src/AacEncoder.cpp
  src/AacFormat.cpp
  src/AdtsParser.cpp
  src/IAudioEncoder.cpp
  src/Mp4Tag.cpp
  src/Mpeg4Audio.cpp
  src/RawEncoder.cpp
  )

### Target ###################################################################

add_library(audiobook STATIC
  ${audiobook_HEADERS}
  ${audiobook_SOURCES}
  )

format_output_name(audiobook "audiobook")

if(MSVC)
  target_compile_options(audiobook
    PRIVATE /std:c++latest
    )
endif()

target_include_directories(audiobook
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(audiobook
  PUBLIC fdk-aac mp4v2
  PUBLIC  csUtil
  )
