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

set(CS_SIGNAL_INCLUDES
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_internal.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_macro.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_signal.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_slot.h

   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_rcu_guarded.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_rcu_list.h
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

install(
   TARGETS CsSignal
   EXPORT CsSignalLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS}
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
   FILES ${CS_SIGNAL_INCLUDES}
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   COMPONENT Devel
)

install(
   EXPORT CsSignalLibraryTargets
   FILE CsSignalLibraryTargets.cmake
   DESTINATION ${PKG_PREFIX}
)
