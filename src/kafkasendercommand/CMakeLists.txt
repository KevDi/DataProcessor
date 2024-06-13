include(generate_product_version)
generate_product_version(
        VersionFilesOutputVariable
        ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/DataProcessor.ico
        NAME KafkaSenderCommand
        VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
        VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
        VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
        VERSION_REVISION ${CPACK_PACKAGE_VERSION_BUILD}
)

add_library("KafkaSenderCommand")
add_library("JKD::DataProcessor::KafkaSenderCommand" ALIAS "KafkaSenderCommand")

set(PUBLIC_HEADER_FILES "include/KafkaSenderCommand/KafkaSenderCommand.hpp"
						"include/KafkaSenderCommand/export.hpp")

target_sources(KafkaSenderCommand
                        PUBLIC
                        ${PUBLIC_HEADER_FILES}
                        PRIVATE
                        "src/KafkaSenderCommand.cpp"
                        "src/KafkaProducerWrapperImpl.cpp"
                        ${VersionFilesOutputVariable})

target_include_directories(KafkaSenderCommand PUBLIC 
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
                            PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(KafkaSenderCommand PROPERTIES
		PUBLIC_HEADER "${PUBLIC_HEADER_FILES}"
		DEBUG_POSTFIX "d"
		SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR} VERSION ${PROJECT_VERSION} 
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
	target_compile_options(KafkaSenderCommand PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(KafkaSenderCommand PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

if(NOT BUILD_SHARED_LIBS)
    string(TOUPPER KafkaSenderCommand PROJECT_NAME_UPPER)
    set_target_properties(KafkaSenderCommand PROPERTIES COMPILE_FLAGS -D${PROJECT_NAME_UPPER}_STATIC)
endif()

find_package(modern-cpp-kafka REQUIRED)

target_link_libraries(KafkaSenderCommand PRIVATE JKD::DataProcessor::Core JKD::DataProcessor::Commands modern-cpp-kafka::modern-cpp-kafka)

include(GenerateExportHeader)
string(TOUPPER KafkaSenderCommand PROJECT_NAME_UPPER)
generate_export_header(KafkaSenderCommand
    EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/KafkaSenderCommand/export.hpp
    EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER}_EXPORT
    STATIC_DEFINE ${PROJECT_NAME_UPPER}_STATIC
)

install(TARGETS KafkaSenderCommand
        PUBLIC_HEADER DESTINATION include/KafkaSenderCommand)

if(ENABLE_TESTING)
	message(STATUS "Running Tests for Library KafkaSenderCommand")
    enable_testing()
    add_subdirectory(tests)
endif()