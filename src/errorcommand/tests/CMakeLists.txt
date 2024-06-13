add_executable(ErrorCommand_TEST)

set(SHARED_TEST_SOURCES "TestRunner.cpp"
                        "ErrorCommandTest.cpp"
                        "ErrorCommandIT.cpp")
						
set(ALL_TEST_SOURCES "${SHARED_TEST_SOURCES}")

if(BUILD_SHARED_LIBS)
    target_sources(ErrorCommand_TEST
                        PRIVATE
                        ${SHARED_TEST_SOURCES}) 
else()
    target_sources(ErrorCommand_TEST
                        PRIVATE
                        ${ALL_TEST_SOURCES}) 
endif()
        

set_target_properties(ErrorCommand_TEST PROPERTIES 
                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)
if(msvc)
	target_compile_options(ErrorCommand_TEST PUBLIC /wd4251 /wd4275)
	set_target_properties(ErrorCommand_TEST PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

find_package(GTest REQUIRED)

target_link_libraries(ErrorCommand_TEST PRIVATE GTest::gmock JKD::DataProcessor::ErrorCommand JKD::DataProcessor::Commands JKD::DataProcessor::Core)
target_include_directories(ErrorCommand_TEST PRIVATE $<TARGET_PROPERTY:JKD::DataProcessor::ErrorCommand,INCLUDE_DIRECTORIES>)

include(GoogleTest)
gtest_discover_tests(ErrorCommand_TEST WORKING_DIRECTORY $<TARGET_FILE_DIR:ErrorCommand_TEST>)