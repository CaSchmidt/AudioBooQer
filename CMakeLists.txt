cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(AudioBooQer-dev)

file(MAKE_DIRECTORY
  ${AudioBooQer-dev_BINARY_DIR}/bin
  ${AudioBooQer-dev_BINARY_DIR}/lib
  )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${AudioBooQer-dev_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${AudioBooQer-dev_BINARY_DIR}/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY     ${AudioBooQer-dev_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AudioBooQer-dev_BINARY_DIR}/bin)

add_subdirectory(AudioBooQer)
