include(generate_product_version)
generate_product_version(
        VersionFilesOutputVariable
        ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/DataProcessor.ico
        NAME Core
        VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
        VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
        VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
        VERSION_REVISION ${CPACK_PACKAGE_VERSION_BUILD}
)

add_library("Core")
add_library("JKD::DataProcessor::Core" ALIAS "Core")

set(PUBLIC_HEADER_FILES "include/Core/export.hpp"
                        "include/Core/Core.hpp"
						"include/Core/Message.hpp"
                        "include/Core/MessageQueue.hpp"
                        "include/Core/MessageQueueImpl.hpp")

target_sources(Core
                        PUBLIC           
                        ${PUBLIC_HEADER_FILES}
                        PRIVATE
                        ${VersionFilesOutputVariable}
                        "src/MessageQueueImpl.cpp"
                        "src/Message.cpp"
                        "src/Core.cpp")

target_include_directories(Core PUBLIC 
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
                            PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(Core PROPERTIES
		PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
		DEBUG_POSTFIX "d"
		SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR} VERSION ${PROJECT_VERSION} 
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
	target_compile_options(Core PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(Core PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

if(NOT BUILD_SHARED_LIBS)
    string(TOUPPER Core PROJECT_NAME_UPPER)
    set_target_properties(Core PROPERTIES COMPILE_FLAGS -D${PROJECT_NAME_UPPER}_STATIC)
endif()

find_package(Poco REQUIRED)
find_package(prometheus-cpp REQUIRED)

target_link_libraries(Core PUBLIC Poco::Foundation)

include(GenerateExportHeader)
string(TOUPPER Core PROJECT_NAME_UPPER)
generate_export_header(Core
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/export.hpp
    EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER}_EXPORT
    STATIC_DEFINE ${PROJECT_NAME_UPPER}_STATIC
)

install(TARGETS Core
        PUBLIC_HEADER DESTINATION include/Core)

if(ENABLE_TESTING)
	message(STATUS "Running Tests for Library Cores")
    enable_testing()
    add_subdirectory(tests)
endif()