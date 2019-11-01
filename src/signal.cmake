add_library(CsSignal SHARED "")

target_compile_definitions(
   CsSignal
   PRIVATE
   -DBUILDING_LIB_CS_SIGNAL
)

target_include_directories(
   CsSignal
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_sources(CsSignal
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_signal.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_slot.cpp
)

set(CS_SIGNAL_INCLUDE
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_internal.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_macro.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_signal.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_slot.h

   ${CMAKE_CURRENT_SOURCE_DIR}/src/rcu_guarded.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/rcu_list.hpp
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

