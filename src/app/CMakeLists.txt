#
# This file is part of the WarheadCore Project. See AUTHORS file for Copyright information
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU Affero General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.
#

# Get all source files
CollectSourceFiles(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE_SOURCES)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

# Add debugging tools for windows
if (WIN32)
  list(APPEND PRIVATE_SOURCES ${winDebugging})
endif()

# Create the application project
add_executable(WarheadImageMirror ${PRIVATE_SOURCES})

target_link_libraries(WarheadImageMirror
  PRIVATE
    warhead-core-interface
  PUBLIC
    server)

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_INCLUDES)

target_include_directories(WarheadImageMirror
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(WarheadImageMirror
  PUBLIC
    ${PUBLIC_INCLUDES})

# Install config
CopyApplicationConfig(${CMAKE_CURRENT_SOURCE_DIR} WarheadImageMirror)

if (UNIX)
  install(TARGETS WarheadImageMirror DESTINATION bin)
elseif (WIN32)
  install(TARGETS WarheadImageMirror DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
