### Target ###################################################################

add_executable(test_adts
  src/test_adts.cpp
  ${AudioBooQer_SOURCE_DIR}/include/adts.h
  ${AudioBooQer_SOURCE_DIR}/src/adts.cpp
  ${AudioBooQer_SOURCE_DIR}/src/mpeg4audio.cpp
  )

target_include_directories(test_adts
  PRIVATE ${AudioBooQer_SOURCE_DIR}/include
  )

target_link_libraries(test_adts Qt5::Core)
