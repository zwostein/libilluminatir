add_subdirectory(ext/Unity)

set(TEST_SOURCES
	src/test_illuminatir_lfsr.c
	src/test_illuminatir_rand.c
	src/test_illuminatir_cobs.c
	src/test_illuminatir_parse.c
	src/test_illuminatir_build.c
)

foreach( TEST_SOURCE ${TEST_SOURCES} )
	get_filename_component( TestName ${TEST_SOURCE} NAME_WE )
	get_filename_component( TestExecutable ${TEST_SOURCE} NAME_WLE )
	add_executable( ${TestExecutable} ${TEST_SOURCE} )
	target_link_libraries( ${TestExecutable} PRIVATE ${CMAKE_PROJECT_NAME} unity )
	add_test( NAME ${TestName} COMMAND ${TestExecutable} )
endforeach()
