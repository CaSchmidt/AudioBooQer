add_library(miniaudio STATIC
  include/miniaudio.h
  include/miniaudio_lib.h
  src/miniaudio_lib.c
  )

target_link_libraries(miniaudio)

format_output_name(miniaudio "miniaudio")

target_include_directories(miniaudio
  PUBLIC
  include
  )
