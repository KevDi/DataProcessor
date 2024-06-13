add_executable(AddMetadataCommand_TEST)

set(SHARED_TEST_SOURCES "TestRunner.cpp"
                        "AddMetadataCommandTest.cpp"
                        "AddMetadataCommandIT.cpp")
						
set(ALL_TEST_SOURCES "${SHARED_TEST_SOURCES}")

if(BUILD_SHARED_LIBS)
    target_sources(AddMetadataCommand_TEST
                        PRIVATE
                        ${SHARED_TEST_SOURCES}) 
else()
    target_sources(AddMetadataCommand_TEST
                        PRIVATE
                        ${ALL_TEST_SOURCES}) 
endif()
        

set_target_properties(AddMetadataCommand_TEST PROPERTIES 
                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)

if(msvc)
	target_compile_options(Commands_TEST PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(Commands_TEST PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

find_package(GTest REQUIRED)

target_link_libraries(AddMetadataCommand_TEST PRIVATE GTest::gmock JKD::DataProcessor::AddMetadataCommand JKD::DataProcessor::Commands JKD::DataProcessor::Core)
target_include_directories(AddMetadataCommand_TEST PRIVATE $<TARGET_PROPERTY:JKD::DataProcessor::AddMetadataCommand,INCLUDE_DIRECTORIES>)

include(GoogleTest)
gtest_discover_tests(AddMetadataCommand_TEST WORKING_DIRECTORY $<TARGET_FILE_DIR:AddMetadataCommand_TEST>)