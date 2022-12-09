add_library(CsSignal SHARED "")
add_library(CsSignal::CsSignal ALIAS CsSignal)

target_compile_definitions(CsSignal
   PRIVATE
   -DBUILDING_LIB_CS_SIGNAL
)

target_compile_features(CsSignal
   PUBLIC
   cxx_std_17
)

if (CsLibGuarded_FOUND)
   # use system headers
   target_link_libraries(CsSignal
      PUBLIC
      CsLibGuarded::CsLibGuarded
   )

   string(TOLOWER "${CS_INSTALL_MODE}" CS_INSTALL_MODE)

   if (NOT CS_INSTALL_MODE STREQUAL "package")
      # deploy mode (default)

      list(APPEND CS_SIGNAL_INCLUDES
         ${CsLibGuarded_INCLUDE_DIR}/cs_rcu_guarded.h
         ${CsLibGuarded_INCLUDE_DIR}/cs_rcu_list.h
      )

   else()
      # package mode, do not copy install headers

   endif()

else()
   # use annex headers
   target_include_directories(CsSignal
      PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/annex/cs_libguarded>
   )

   list(APPEND CS_SIGNAL_INCLUDES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/annex/cs_libguarded/cs_rcu_guarded.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/annex/cs_libguarded/cs_rcu_list.h
   )

endif()

target_include_directories(CsSignal
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/signal>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_sources(CsSignal
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/src/signal/cs_signal.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/signal/cs_slot.cpp
)

list(APPEND CS_SIGNAL_INCLUDES
   ${CMAKE_CURRENT_SOURCE_DIR}/src/signal/cs_internal.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/signal/cs_macro.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/signal/cs_signal.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/signal/cs_slot.h
)

if(MSVC)
   if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.0")
      # ensure method pointers have a unique address
      target_link_options(CsSignal
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
   COMPONENT CsSignal
)

install(
   EXPORT CsSignalLibraryTargets
   NAMESPACE CsSignal::
   FILE CsSignalLibraryTargets.cmake
   DESTINATION ${PKG_PREFIX}
)
