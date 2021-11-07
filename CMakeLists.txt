cmake_minimum_required(VERSION 3.12)
project(illuminatir C)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(CODE_COVERAGE "Enable code coverage information collection" OFF)
if(CODE_COVERAGE)
	include(CodeCoverage)
	append_coverage_compiler_flags()
	setup_target_for_coverage_lcov(
		NAME coverage
		EXECUTABLE make test
		EXCLUDE "${PROJECT_SOURCE_DIR}/test/*"
	)
endif()

set(CMAKE_C_STANDARD 11)

add_compile_options(
	-Wall
	-Wextra
)

include_directories(
	${PROJECT_SOURCE_DIR}/include/
)

set(SOURCES
	${PROJECT_SOURCE_DIR}/src/illuminatir.c
	${PROJECT_SOURCE_DIR}/src/crc8.c
	${PROJECT_SOURCE_DIR}/src/cobs.c
	${PROJECT_SOURCE_DIR}/src/lfsr.c
	${PROJECT_SOURCE_DIR}/src/rand.c
)

add_library( ${PROJECT_NAME} ${SOURCES} )


option(DOCUMENTATION "Enable generation of documentation" OFF)
if(DOCUMENTATION)
	find_package(Doxygen
		REQUIRED dot
		OPTIONAL_COMPONENTS mscgen dia
	)

	set(DOXYGEN_PROJECT_NAME libIlluminatIR)
	set(DOXYGEN_PROJECT_BRIEF "A generic library implementing the IlluminatIR protocol.")
	set(DOXYGEN_EXAMPLE_PATH "example")
	set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
	doxygen_add_docs(doxygen
		${PROJECT_SOURCE_DIR}/include/
		${PROJECT_SOURCE_DIR}/README.md
	)

	set(EXAMPLE_SOURCES
		example/example_parse_minimal.c
		example/example_cobs_build.c
	)
	foreach( EXAMPLE_SOURCE ${EXAMPLE_SOURCES} )
		get_filename_component( ExampleExecutable ${EXAMPLE_SOURCE} NAME_WLE )
		add_executable( ${ExampleExecutable} ${EXAMPLE_SOURCE} )
		target_link_libraries( ${ExampleExecutable} PRIVATE ${CMAKE_PROJECT_NAME} )
	endforeach()
endif()


include( CTest )
if( BUILD_TESTING )
	add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()
