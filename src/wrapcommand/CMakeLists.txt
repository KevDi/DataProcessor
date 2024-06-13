include(generate_product_version)
generate_product_version(
        VersionFilesOutputVariable
        ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/DataProcessor.ico
        NAME WrapCommand
        VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
        VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
        VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
        VERSION_REVISION ${CPACK_PACKAGE_VERSION_BUILD}
)

add_library("WrapCommand")
add_library("JKD::DataProcessor::WrapCommand" ALIAS "WrapCommand")

set(PUBLIC_HEADER_FILES "include/WrapCommand/WrapCommand.hpp"
						"include/WrapCommand/export.hpp")

target_sources(WrapCommand
                        PUBLIC
                        ${PUBLIC_HEADER_FILES}
                        PRIVATE
                        "src/WrapCommand.cpp"
                        ${VersionFilesOutputVariable}
  )

target_include_directories(WrapCommand PUBLIC 
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
                            PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(WrapCommand PROPERTIES
		PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
		DEBUG_POSTFIX "d"
		SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR} VERSION ${PROJECT_VERSION} 
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
	target_compile_options(WrapCommand PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(WrapCommand PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

if(NOT BUILD_SHARED_LIBS)
    string(TOUPPER WrapCommand PROJECT_NAME_UPPER)
    set_target_properties(WrapCommand PROPERTIES COMPILE_FLAGS -D${PROJECT_NAME_UPPER}_STATIC)
endif()

find_package(nlohmann_json REQUIRED)

target_link_libraries(WrapCommand PRIVATE JKD::DataProcessor::Core JKD::DataProcessor::Commands nlohmann_json::nlohmann_json)

include(GenerateExportHeader)
string(TOUPPER WrapCommand PROJECT_NAME_UPPER)
generate_export_header(WrapCommand
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/WrapCommand/export.hpp
    EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER}_EXPORT
    STATIC_DEFINE ${PROJECT_NAME_UPPER}_STATIC
)

install(TARGETS WrapCommand
        PUBLIC_HEADER DESTINATION include/WrapCommand)

if(ENABLE_TESTING)
	message(STATUS "Running Tests for Library WrapCommand")
    enable_testing()
    add_subdirectory(tests)
endif()