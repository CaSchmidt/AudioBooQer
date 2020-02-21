### Target ###################################################################

add_executable(test_adts
  src/test_adts.cpp
  )

target_link_libraries(test_adts audiobook csUtil)
