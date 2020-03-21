### Project ##################################################################

list(APPEND ui_FORMS
  forms/WAudioFormat.ui
  forms/WAudioPlayer.ui
  forms/WBookBinder.ui
  forms/WJobInfo.ui
  forms/WMainWindow.ui
  forms/WTagEditor.ui
  )

list(APPEND ui_HEADERS
  include/AudioJob.h
  include/BinderIO.h
  include/BookBinderModel.h
  include/Chapter.h
  include/ChapterModel.h
  include/Job.h
  include/WAudioFormat.h
  include/WAudioPlayer.h
  include/WBookBinder.h
  include/WJobInfo.h
  include/WMainWindow.h
  include/WTagEditor.h
  )

list(APPEND ui_SOURCES
  src/AudioJob.cpp
  src/BinderIO.cpp
  src/BookBinderModel.cpp
  src/Chapter.cpp
  src/ChapterModel.cpp
  src/Job.cpp
  src/main.cpp
  src/WAudioFormat.cpp
  src/WAudioPlayer.cpp
  src/WBookBinder.cpp
  src/WJobInfo.cpp
  src/WMainWindow.cpp
  src/WTagEditor.cpp
  )

list(APPEND ui_RESOURCES
  resources/resources.qrc
  )

### Target ###################################################################

add_executable(ui
  ${ui_FORMS}
  ${ui_HEADERS}
  ${ui_SOURCES}
  ${ui_RESOURCES}
  )

format_output_name(ui "AudioBooQer")

set_target_properties(ui PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
  AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms
  )

if(WIN32)
  set_target_properties(ui PROPERTIES
    WIN32_EXECUTABLE ON
    )
endif()

target_compile_definitions(ui
  PRIVATE -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII
  )

target_include_directories(ui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(ui PRIVATE ${csQt_SOURCE_DIR}/include)
target_include_directories(ui PRIVATE ${csQt_BINARY_DIR}/include)

target_link_libraries(ui audiobook csQt Qt5::Widgets Qt5::Multimedia Qt5::Concurrent)

install(TARGETS ui
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )