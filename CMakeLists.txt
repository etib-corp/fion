cmake_minimum_required(VERSION 3.10)

project(fion
    VERSION 1.0.0
    DESCRIPTION "Framework with Intelligent ORM and Networking"
    HOMEPAGE_URL "https://github.com/etib-corp/fion"
    LANGUAGES CXX
)

# Include common functions
include(cmake/CommonFunctions.cmake)

# Set global C++ standard and properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE
        CMAKE_BUILD_TYPE PROPERTY
        STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Feature options
option(BUILD_FOR_OPENXR "Build with OpenXR support" OFF)
option(BUILD_FOR_GLFW "Build with GLFW support" OFF)
option(BUILD_SHARED_LIBS "Build as shared library" OFF)

# Set the files directories
set(HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/headers")
set(SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sources")

# Add the headers directories
include_directories(${HEADERS_DIR})

# Collect common source files (auto-detect new files without manual reconfigure)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SOURCES_DIR}/*.cpp")

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()

# Set common target properties
set_fion_target_properties(${PROJECT_NAME})

# Configure public interface
target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${HEADERS_DIR}> $<INSTALL_INTERFACE:include>)

# Optional components
option(BUILD_EXAMPLES "Build the examples" ON)

if(BUILD_EXAMPLES)
    message(STATUS "Building examples...")
    add_subdirectory(examples)
else()
    message(STATUS "Skipping examples...")
endif()
