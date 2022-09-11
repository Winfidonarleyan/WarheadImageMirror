#
# This file is part of the WarheadCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

find_package(JPEG REQUIRED)

if (NOT JPEG_FOUND)
    message(FATAL_ERROR "Libjpeg wasn't found on your system but it's required to build the server!")
endif()

add_library(libjpeg INTERFACE)

target_link_libraries(libjpeg
    INTERFACE
    ${JPEG_LIBRARIES})

target_include_directories(libjpeg
    INTERFACE
    ${JPEG_INCLUDE_DIRS})