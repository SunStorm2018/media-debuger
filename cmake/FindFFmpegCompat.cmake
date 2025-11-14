# SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
# SPDX-License-Identifier: MIT

# FindFFmpegCompat.cmake - FFmpeg compatibility finder module

# Use pkg-config to find FFmpeg libraries
find_package(PkgConfig REQUIRED)

# Find all required FFmpeg components
set(FFMPEG_COMPONENTS
    libavcodec
    libavformat
    libavutil
    libswscale
    libswresample
    libavfilter
)

# Initialize variables
set(FFMPEG_FOUND TRUE)
set(FFMPEG_LIBRARIES "")
set(FFMPEG_INCLUDE_DIRS "")
set(FFMPEG_CFLAGS_OTHER "")

# Find components one by one
foreach(COMPONENT ${FFMPEG_COMPONENTS})
    pkg_check_modules(PC_${COMPONENT} REQUIRED ${COMPONENT})
    
    if(PC_${COMPONENT}_FOUND)
        list(APPEND FFMPEG_LIBRARIES ${PC_${COMPONENT}_LIBRARIES})
        list(APPEND FFMPEG_INCLUDE_DIRS ${PC_${COMPONENT}_INCLUDE_DIRS})
        list(APPEND FFMPEG_CFLAGS_OTHER ${PC_${COMPONENT}_CFLAGS_OTHER})
        message(STATUS "Found ${COMPONENT}: ${PC_${COMPONENT}_LIBRARIES}")
    else()
        set(FFMPEG_FOUND FALSE)
        message(FATAL_ERROR "FFmpeg component ${COMPONENT} not found")
    endif()
endforeach()

# Remove duplicates from lists
list(REMOVE_DUPLICATES FFMPEG_LIBRARIES)
list(REMOVE_DUPLICATES FFMPEG_INCLUDE_DIRS)
list(REMOVE_DUPLICATES FFMPEG_CFLAGS_OTHER)

# Create imported target (if it doesn't exist)
if(NOT TARGET FFMpeg::FFmpeg)
    add_library(FFMpeg::FFmpeg INTERFACE IMPORTED)
    
    set_target_properties(FFMpeg::FFmpeg PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${FFMPEG_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${FFMPEG_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${FFMPEG_CFLAGS_OTHER}"
    )
endif()

# Export variables to parent scope
set(FFMPEG_FOUND ${FFMPEG_FOUND} PARENT_SCOPE)
set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} PARENT_SCOPE)
set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIRS} PARENT_SCOPE)
set(FFMPEG_CFLAGS_OTHER ${FFMPEG_CFLAGS_OTHER} PARENT_SCOPE)

# Print information
message(STATUS "FFmpeg configuration:")
message(STATUS "  Found: ${FFMPEG_FOUND}")
message(STATUS "  Libraries: ${FFMPEG_LIBRARIES}")
message(STATUS "  Include dirs: ${FFMPEG_INCLUDE_DIRS}")
message(STATUS "  Compile flags: ${FFMPEG_CFLAGS_OTHER}")