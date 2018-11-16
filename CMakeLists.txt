cmake_minimum_required(VERSION 3.0.2)

# Build Options
option(BUILD_BUNDLE     "Build bundle library"                              ON)
option(BUILD_EXAMPLES   "Build examples directory"                          OFF)
option(BUILD_TESTS      "Build tests directory"                             OFF)
option(STRICT_COMPILE   "Enable all compiler warnings and treat as error"   OFF)

# Project setup
project(live
	VERSION 2018.10.17
	LANGUAGES C CXX
)

# Turn On -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-ffunction-sections COMPILER_SUPPORTS_FFUNCTION_SECTION)
CHECK_CXX_COMPILER_FLAG(-fdata-sections COMPILER_SUPPORTS_FDATA_SECTION)

if(COMPILER_SUPPORTS_FFUNCTION_SECTION)
	add_compile_options(-ffunction-sections)
endif()

if(COMPILER_SUPPORTS_FDATA_SECTION)
	add_compile_options(-fdata-sections)
endif()

if(STRICT_COMPILE)
	add_compile_options(-Wall -Werror)
endif()

if(COMPILER_SUPPORTS_FFUNCTION_SECTION OR COMPILER_SUPPORTS_FDATA_SECTION)
	list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
endif()

#add_compile_options(-fno-exceptions)
#add_compile_options(-fno-rtti)

add_definitions(-DSOCKLEN_T=socklen_t)

add_subdirectory(BasicUsageEnvironment)
add_subdirectory(groupsock)
add_subdirectory(liveMedia)
add_subdirectory(UsageEnvironment)

if(BUILD_EXAMPLES)
	add_subdirectory(proxyServer)
	add_subdirectory(mediaServer)
endif()

if(BUILD_TESTS)
	add_subdirectory(testProgs)
endif()

# Bundle to single library
if(BUILD_BUNDLE)
	file(GLOB SOURCES
		BasicUsageEnvironment/*.cpp
		BasicUsageEnvironment/*.c
		groupsock/*.cpp
		groupsock/*.c
		liveMedia/*.cpp
		liveMedia/*.c
		UsageEnvironment/*.cpp
		UsageEnvironment/*.c
	)

	include_directories(
		${CMAKE_SOURCE_DIR}/BasicUsageEnvironment/include
		${CMAKE_SOURCE_DIR}/groupsock/include
		${CMAKE_SOURCE_DIR}/liveMedia/include
		${CMAKE_SOURCE_DIR}/UsageEnvironment/include
	)

	add_library(${PROJECT_NAME} ${SOURCES})

	install(TARGETS ${PROJECT_NAME}
		ARCHIVE DESTINATION lib
	)
endif()
