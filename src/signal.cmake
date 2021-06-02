add_library(CsSignal SHARED "")

target_compile_definitions(
   CsSignal
   PRIVATE
   -DBUILDING_LIB_CS_SIGNAL
)

target_compile_features(
   CsSignal
   PUBLIC
   cxx_std_17
)

target_sources(CsSignal
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_signal.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_slot.cpp
)

target_include_directories(
   CsSignal
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

if(MSVC)
   if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.0")
      # ensure method pointers have a unique address
      target_link_options(CsCore
         PUBLIC
         /OPT:REF,NOICF
      )
   else()
      message(FATAL_ERROR "CMake Version must be at least 3.13.0 for MSVC")

   endif()
endif()

set(CS_SIGNAL_INCLUDE
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_internal.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_macro.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_signal.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_slot.h

   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_rcu_guarded.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_rcu_list.h
)

install(
   TARGETS CsSignal
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
   FILES ${CS_SIGNAL_INCLUDE}
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   COMPONENT Devel
)

