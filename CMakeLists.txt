cmake_minimum_required(VERSION 3.0.2)

# Build Options
option(BUILD_BUNDLE     "Build bundle library"                              ON)
option(BUILD_EXAMPLES   "Build examples directory"                          OFF)
option(BUILD_TESTS      "Build tests directory"                             OFF)
option(STRICT_COMPILE   "Enable all compiler warnings and treat as error"   OFF)
option(WITH_OPENSSL     "Enalble openssl"                                   OFF)

# Project setup
project(live
	VERSION 2022.04.15
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
add_definitions(-DALLOW_SERVER_PORT_REUSE=1)
add_definitions(-DALLOW_RTSP_SERVER_PORT_REUSE=1)
add_definitions(-DREUSE_FOR_TCP=1)
if(NOT WITH_OPENSSL)
add_definitions(-DNO_OPENSSL=1)
endif()

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

# Installation (https://github.com/forexample/package-example)

set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
