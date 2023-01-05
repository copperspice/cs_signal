# ***********************************************************************
#
# Copyright (c) 2016-2023 Barbara Geller
# Copyright (c) 2016-2023 Ansel Sermersheim
#
# This file is part of CsSignal.
#
# CsSignal is free software, released under the BSD 2-Clause license.
# For license details refer to LICENSE provided with this project.
#
# CsSignal is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# https://opensource.org/licenses/BSD-2-Clause
#
# ***********************************************************************

if(CsSignal_FOUND)
   return()
endif()

set(CsSignal_FOUND TRUE)

# figure out install path
get_filename_component(CsSignal_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
get_filename_component(CsSignal_PREFIX ${CsSignal_CMAKE_DIR}/ ABSOLUTE)

# library dependencies (contains definitions for imported targets)
include("${CsSignal_CMAKE_DIR}/CsSignalLibraryTargets.cmake")

# imported targets INCLUDE_DIRECTORIES
get_target_property(CsSignal_INCLUDES  CsSignal::CsSignal INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(CsSignal_LIBRARIES CsSignal::CsSignal LOCATION)

# export include base dir, imported in other projects
set(CsSignal_INCLUDE_DIR  "@CMAKE_INSTALL_PREFIX@/@CMAKE_INSTALL_INCLUDEDIR@")
