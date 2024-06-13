include(generate_product_version)
generate_product_version(
        VersionFilesOutputVariable
        ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/DataProcessor.ico
        NAME Commands
        VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
        VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
        VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
        VERSION_REVISION ${CPACK_PACKAGE_VERSION_BUILD}
)

add_library("Commands")
add_library("JKD::DataProcessor::Commands" ALIAS "Commands")

set(PUBLIC_HEADER_FILES "include/Commands/Command.hpp"      
                        "include/Commands/CommandFactory.hpp"
						"include/Commands/export.hpp")

target_sources(Commands PUBLIC
						${PUBLIC_HEADER_FILES}
                        PRIVATE
                        "src/Command.cpp"
                        "src/CommandFactory.cpp"
                        ${VersionFilesOutputVariable}
  )

target_include_directories(Commands PUBLIC 
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
                            PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(Commands PROPERTIES
		PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
		DEBUG_POSTFIX "d"
		SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR} VERSION ${PROJECT_VERSION} 
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
	target_compile_options(Commands PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(Commands PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

if(NOT BUILD_SHARED_LIBS)
    string(TOUPPER Commands PROJECT_NAME_UPPER)
    set_target_properties(Commands PROPERTIES COMPILE_FLAGS -D${PROJECT_NAME_UPPER}_STATIC)
endif()

target_link_libraries(Commands PUBLIC JKD::DataProcessor::Core)

include(GenerateExportHeader)
string(TOUPPER Commands PROJECT_NAME_UPPER)
generate_export_header(Commands
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/Commands/export.hpp
    EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER}_EXPORT
    STATIC_DEFINE ${PROJECT_NAME_UPPER}_STATIC
)

install(TARGETS Commands
        PUBLIC_HEADER DESTINATION include/Commands)

if(ENABLE_TESTING)
	message(STATUS "Running Tests for Library Commands")
    enable_testing()
    add_subdirectory(tests)
endif()