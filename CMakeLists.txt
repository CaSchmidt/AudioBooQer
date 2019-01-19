cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(AudioBooQer-dev)

add_subdirectory(AudioBooQer)

set_target_properties(csQt PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${AudioBooQer_BINARY_DIR}
  )
