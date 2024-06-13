cmake_minimum_required(VERSION 3.14)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source")
endif()

project("DataProcessor" LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_SHARED_LIBS "Build shared (dynamic) libraries." ON)
option(ENABLE_TESTING "Build with Tests enabled." OFF)
option(BUILD_COMMANDS "Building the Commands." ON)
option(BUILD_INPUTS "Build the input Sources." ON)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PACKAGE_VERSION)

if(${PACKAGE_VERSION} MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)((-dev)?|-dev(\\.([0-9]+)))?$")
	set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_MATCH_1})
	set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_MATCH_2})
	set(CPACK_PACKAGE_VERSION_PATCH ${CMAKE_MATCH_3})
	if (NOT DEFINED CMAKE_MATCH_7)
		set(CPACK_PACKAGE_VERSION_BUILD "0")
	else()
		set(CPACK_PACKAGE_VERSION_BUILD ${CMAKE_MATCH_7})
	endif()
else() 
	message(FATAL_ERROR "Project Version Format does not match. Format: MAJOR.MINOR.PATCH-dev[.BUILD_NUMBER]! Examples: 1.2.3-dev.13, 1.2.3, 1.2.3-dev")
endif()
		
set(PROJECT_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

if(ENABLE_TESTING) 
    include(CTest)
	enable_testing()
endif()

add_subdirectory(src)