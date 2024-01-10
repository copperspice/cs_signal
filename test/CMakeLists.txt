include(Load_Catch2)
include(CTest)

find_package(Threads)

if (Threads_FOUND)

   add_executable(CsSignalTest "")
   set_target_properties(CsSignalTest
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
   )

   if (MSVC)
      # CopperSpice must be compiled with C++20 in order to run the unit test for C++20

   else()
      set_target_properties(CsSignalTest
         PROPERTIES

         # only a request for C++20
         CXX_STANDARD_REQUIRED OFF
         CXX_STANDARD 20
      )

   endif()

   file(WRITE ${CMAKE_BINARY_DIR}/bin/test/run_test.bat
      "@SET PATH=..;%PATH%\n@CsSignalTest\n"
   )

   target_link_libraries(CsSignalTest
      PUBLIC
      CsSignal
      Catch2::Catch2
      Threads::Threads
   )

   include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}
   )

   target_sources(CsSignalTest
      PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/cs_catch2.h

      ${CMAKE_CURRENT_SOURCE_DIR}/cs_signal.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/cs_slot.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/demo.h
      ${CMAKE_CURRENT_SOURCE_DIR}/demo.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
   )

   include(ParseAndAddCatchTests)
   ParseAndAddCatchTests(CsSignalTest)
endif()
