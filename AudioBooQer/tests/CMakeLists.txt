### Target ###################################################################

add_executable(test_adts
  src/test_adts.cpp
  ${AudioBooQer_SOURCE_DIR}/audiobook/include/AdtsParser.h
  ${AudioBooQer_SOURCE_DIR}/audiobook/src/AdtsParser.cpp
  ${AudioBooQer_SOURCE_DIR}/audiobook/include/Mpeg4Audio.h
  ${AudioBooQer_SOURCE_DIR}/audiobook/src/Mpeg4Audio.cpp
  )

if(MSVC)
  target_compile_options(test_adts
    PRIVATE /std:c++latest
    )
endif()

target_include_directories(test_adts
  PRIVATE ${AudioBooQer_SOURCE_DIR}/audiobook/include
  )

target_link_libraries(test_adts csUtil)
