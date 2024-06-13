add_executable(FilesystemInput_TEST)

set(SHARED_TEST_SOURCES "TestRunner.cpp"
						"FilesystemInputTest.cpp"
						"FilesystemInputIT.cpp")
						
set(ALL_TEST_SOURCES "${SHARED_TEST_SOURCES}"
					 "DirectoryReaderTest.cpp"
					 "WorkerThreadTest.cpp"
					 "FileReaderImplTest.cpp"
					 "FileNotificationTest.cpp" 
					 "DirectoryWatcherTest.cpp"
					 "FilesystemInputTest.cpp")

if(BUILD_SHARED_LIBS)
	target_sources(FilesystemInput_TEST
		PRIVATE
		"${SHARED_TEST_SOURCES}"
	)
else()
	target_sources(FilesystemInput_TEST
		PRIVATE
		"${ALL_TEST_SOURCES}"
	)
endif()

set_target_properties(FilesystemInput_TEST PROPERTIES 
                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)

if(msvc)
	target_compile_options(FilesystemInput_TEST PUBLIC /wd4251 /wd4275 PRIVATE /W4)
	set_target_properties(FilesystemInput_TEST PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

find_package(GTest REQUIRED)

target_link_libraries(FilesystemInput_TEST PRIVATE GTest::gmock JKD::DataProcessor::Core JKD::DataProcessor::FilesystemInput)

target_include_directories(FilesystemInput_TEST PRIVATE $<TARGET_PROPERTY:JKD::DataProcessor::FilesystemInput,INCLUDE_DIRECTORIES>)

add_custom_command(
    TARGET FilesystemInput_TEST POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            ${CMAKE_BINARY_DIR}/bin/test/$<CONFIG>/resources
)

include(GoogleTest)
gtest_discover_tests(FilesystemInput_TEST WORKING_DIRECTORY $<TARGET_FILE_DIR:FilesystemInput_TEST>)