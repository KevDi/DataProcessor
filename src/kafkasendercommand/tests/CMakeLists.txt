add_executable(KafkaSenderCommand_TEST)

set(SHARED_TEST_SOURCES "TestRunner.cpp"
                        "KafkaSenderCommandTest.cpp")
						
set(ALL_TEST_SOURCES "${SHARED_TEST_SOURCES}")

if(BUILD_SHARED_LIBS)
    target_sources(KafkaSenderCommand_TEST
                        PRIVATE
                        ${SHARED_TEST_SOURCES}) 
else()
    target_sources(KafkaSenderCommand_TEST
                        PRIVATE
                        ${ALL_TEST_SOURCES}) 
endif()
        

set_target_properties(KafkaSenderCommand_TEST PROPERTIES 
                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)
if(msvc)
	target_compile_options(KafkaSenderCommand_TEST PUBLIC /wd4251 /wd4275)
	set_target_properties(KafkaSenderCommand_TEST PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

find_package(GTest REQUIRED)

target_link_libraries(KafkaSenderCommand_TEST PRIVATE GTest::gmock JKD::DataProcessor::KafkaSenderCommand JKD::DataProcessor::Commands JKD::DataProcessor::Core modern-cpp-kafka::modern-cpp-kafka)
target_include_directories(KafkaSenderCommand_TEST PRIVATE $<TARGET_PROPERTY:JKD::DataProcessor::KafkaSenderCommand,INCLUDE_DIRECTORIES>)

include(GoogleTest)
gtest_discover_tests(KafkaSenderCommand_TEST WORKING_DIRECTORY $<TARGET_FILE_DIR:KafkaSenderCommand_TEST> )