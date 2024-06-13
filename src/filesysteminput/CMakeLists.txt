add_library("FilesystemInput")
add_library("JKD::DataProcessor::FilesystemInput" ALIAS "FilesystemInput")

set(PUBLIC_HEADER_FILES "include/FilesystemInput/FilesystemInput.hpp"
						"include/FilesystemInput/export.hpp")


target_sources(FilesystemInput
                        PUBLIC   
                        ${PUBLIC_HEADER_FILES}
                        PRIVATE
                        "src/FilesystemInput.cpp"
                        )


target_include_directories(FilesystemInput PUBLIC 
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
                            PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(FilesystemInput PROPERTIES
		PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
		DEBUG_POSTFIX "d"
		SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR} VERSION ${PROJECT_VERSION} 
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
	target_compile_options(FilesystemInput PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(FilesystemInput PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

if(NOT BUILD_SHARED_LIBS)
    string(TOUPPER FilesystemInput PROJECT_NAME_UPPER)
    set_target_properties(FilesystemInput PROPERTIES COMPILE_FLAGS -D${PROJECT_NAME_UPPER}_STATIC)
endif()

target_link_libraries(FilesystemInput PUBLIC JKD::DataProcessor::Core)

include(GenerateExportHeader)
string(TOUPPER FilesystemInput PROJECT_NAME_UPPER)
generate_export_header(FilesystemInput
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/FilesystemInput/export.hpp
    EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER}_EXPORT
    STATIC_DEFINE ${PROJECT_NAME_UPPER}_STATIC
)

install(TARGETS FilesystemInput
        PUBLIC_HEADER DESTINATION include/FilesystemInput)
