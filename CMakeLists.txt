cmake_minimum_required(VERSION 3.10)

project(live
	VERSION 2018.07.07
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

list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")

#add_compile_options(-fno-exceptions)
#add_compile_options(-fno-rtti)

add_definitions(-DSOCKLEN_T=socklen_t)

add_subdirectory(BasicUsageEnvironment)
add_subdirectory(groupsock)
add_subdirectory(liveMedia)
add_subdirectory(UsageEnvironment)
add_subdirectory(proxyServer)
add_subdirectory(mediaServer)
add_subdirectory(testProgs)

# Bundle to single library

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