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
	include/
)

set(SOURCES
	${PROJECT_SOURCE_DIR}/src/illuminatir.c
	${PROJECT_SOURCE_DIR}/src/crc8.c
	${PROJECT_SOURCE_DIR}/src/cobs.c
)

add_library( ${PROJECT_NAME} ${SOURCES} )


include( CTest )
if( BUILD_TESTING )
	add_subdirectory(test)
endif()
