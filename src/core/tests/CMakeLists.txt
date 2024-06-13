add_executable(Core_TEST)

set(SHARED_TEST_SOURCES "TestRunner.cpp"
                        "MessageTest.cpp"
                        "MessageQueueTest.cpp")

set(ALL_TEST_SOURCES "${SHARED_TEST_SOURCES}")

if(BUILD_SHARED_LIBS)
    target_sources(Core_TEST
                        PRIVATE
                        "${SHARED_TEST_SOURCES}"
    ) 
else()
    target_sources(Core_TEST
                        PRIVATE
                        "${SHARED_TEST_SOURCES}"
    ) 
endif()
        
set_target_properties(Core_TEST PROPERTIES 
                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)

if(msvc)
	target_compile_options(Core_TEST PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(Core_TEST PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

find_package(GTest REQUIRED)
target_link_libraries(Core_TEST PRIVATE GTest::gmock JKD::DataProcessor::Core Poco::Foundation)

target_include_directories(Core_TEST PRIVATE $<TARGET_PROPERTY:JKD::DataProcessor::Core,INCLUDE_DIRECTORIES>)

include(GoogleTest)
gtest_discover_tests(Core_TEST WORKING_DIRECTORY $<TARGET_FILE_DIR:Core_TEST>)