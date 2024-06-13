include(generate_product_version)
generate_product_version(
        VersionFilesOutputVariable
        ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/DataProcessor.ico
        NAME AddMetadataCommand
        VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
        VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
        VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
        VERSION_REVISION ${CPACK_PACKAGE_VERSION_BUILD}
)

add_library("AddMetadataCommand")
add_library("JKD::DataProcessor::AddMetadataCommand" ALIAS "AddMetadataCommand")

set(PUBLIC_HEADER_FILES "include/AddMetadataCommand/AddMetadataCommand.hpp"
						"include/AddMetadataCommand/export.hpp")

target_sources(AddMetadataCommand
                        PUBLIC           
						${PUBLIC_HEADER_FILES}
                        PRIVATE
                        "src/AddMetadataCommand.cpp"
                        ${VersionFilesOutputVariable}
  )

target_include_directories(AddMetadataCommand PUBLIC 
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
                            PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(AddMetadataCommand PROPERTIES
		PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
		DEBUG_POSTFIX "d"
		SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR} VERSION ${PROJECT_VERSION} 
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
	target_compile_options(AddMetadataCommand PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(AddMetadataCommand PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

if(NOT BUILD_SHARED_LIBS)
    string(TOUPPER AddMetadataCommand PROJECT_NAME_UPPER)
    set_target_properties(AddMetadataCommand PROPERTIES COMPILE_FLAGS -D${PROJECT_NAME_UPPER}_STATIC)
endif()

target_link_libraries(AddMetadataCommand PRIVATE JKD::DataProcessor::Core JKD::DataProcessor::Commands)

include(GenerateExportHeader)
string(TOUPPER AddMetadataCommand PROJECT_NAME_UPPER)
generate_export_header(AddMetadataCommand
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/AddMetadataCommand/export.hpp
    EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER}_EXPORT
    STATIC_DEFINE ${PROJECT_NAME_UPPER}_STATIC
)

install(TARGETS AddMetadataCommand
        PUBLIC_HEADER DESTINATION include/AddMetadataCommand)

if(ENABLE_TESTING)
	message(STATUS "Running Tests for Library AddMetadataCommand")
    enable_testing()
    add_subdirectory(tests)
endif()