add_executable(Test "")

target_sources(Test
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/test/peach.h

   ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/test/peach.cpp
)

target_link_libraries(Test
   PRIVATE
   CsSignal
)