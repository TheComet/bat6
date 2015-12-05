cmake_minimum_required (VERSION 2.6)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition and dump it in the cache along with proper documentation, otherwise
# set MAKE_BUILD_TYPE to Debug prior to calling PROJECT()
if (DEFINED CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
else()
    set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif()
message (STATUS "Build type: ${CMAKE_BUILD_TYPE}")

###############################################################################
# Project setup
###############################################################################

project ("unit_tests")

# Make macs happy
set (CMAKE_MACOSX_RPATH OFF)

# determine platform - this is used to replace @PLATFORM@ in the various
# export.h.in files.
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (PLATFORM "LINUX")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (PLATFORM "MACOSX")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (PLATFORM "WINDOWS")
endif ()

# Create vcproj.user file for Visual Studio to set debug working directory
# correctly.
# (Thanks David Avedissian)
function(create_vcproj_userfile TARGETNAME)
  if(MSVC)
    configure_file(
      ${CMAKE_SOURCE_DIR}/cmake/templates/VisualStudioUserFile.vcproj.user.in
      ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcproj.user
      @ONLY)
    configure_file(
      ${CMAKE_SOURCE_DIR}/cmake/templates/VisualStudioUserFile.vcxproj.user.in
      ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcxproj.user
      @ONLY)
  endif()
endfunction()

###############################################################################
# set output locations
###############################################################################

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
if (WIN32)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else ()
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif ()

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

###############################################################################
# compiler definitions and flags
###############################################################################

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    set (DEFINE_CMD -D)
    add_definitions (-W -Wall -Wextra -Werror -Wno-unused-parameter -Wno-missing-field-initializers)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    set (DEFINE_CMD -D)
    add_definitions (-W -Wall -Wextra -Werror -Wno-unused-parameter -Wno-missing-field-initializers)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
    set (DEFINE_CMD /D)
    add_definitions (/D _CRT_SECURE_NO_WARNINGS)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        STRING(REGEX REPLACE "/MDd" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
        add_definitions (/MTd)
    elseif (CMAKE_BUILD_TYPE MATCHES Release)
        STRING(REGEX REPLACE "/MD" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
        add_definitions (/MT)
    endif ()
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(
        ${DEFINE_CMD}_DEBUG
        ${DEFINE_CMD}DEBUG
    )
endif ()
if (CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(${DEFINE_CMD}NDEBUG)
endif ()

###############################################################################
# Target setup
###############################################################################

add_definitions (${DEFINE_CMD}TESTING)

file (GLOB tests_SOURCES "src/*.cpp")
file (GLOB_RECURSE tests_dsPIC_SOURCES
    RELATIVE "${CMAKE_SOURCE_DIR}"
            "${CMAKE_SOURCE_DIR}/../dspic/src/*.c")
file (GLOB_RECURSE tests_dsPIC_HEADERS
	RELATIVE "${CMAKE_SOURCE_DIR}"
			"${CMAKE_SOURCE_DIR}/../dspic/include/*.h")

set_source_files_properties (${tests_dsPIC_SOURCES} PROPERTIES LANGUAGE CXX)

include_directories ("${CMAKE_SOURCE_DIR}/../dspic/include")

add_executable (unit_tests
    ${tests_SOURCES}
    ${tests_dsPIC_SOURCES}
    ${tests_dsPIC_HEADERS}
)

target_link_libraries (unit_tests
    dspic_emulation
    gmock_main
)

# generates the correct project files for visual studio, setting the correct
# debug working directory and other stuff
create_vcproj_userfile (unit_tests)

###############################################################################
# Dependencies
###############################################################################

message (STATUS "------------------------------------------------------------")
message (STATUS "Global settings")
message (STATUS " + Platform is: ${PLATFORM}")
message (STATUS "------------------------------------------------------------")

add_subdirectory ("gmock")
add_subdirectory ("dspic_emulation")

include_directories ("gmock/include")
include_directories ("gmock/gtest/include")
include_directories ("dspic_emulation/include")
