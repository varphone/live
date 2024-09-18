cmake_minimum_required(VERSION 3.21)

project(live)

# options
#===============================================================================
option(ENABLE_TRACE "Enable trace" OFF)

# compiler settings
#===============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(
    SOCKLEN_T=socklen_t
    _LARGEFILE_SOURCE=1
    _FILE_OFFSET_BITS=64
)

if(ENABLE_TRACE)
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
else()
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
endif()

# third-party libraries
#===============================================================================
find_package(OpenSSL CONFIG)
if(OpenSSL_FOUND)
    link_libraries(OpenSSL::SSL OpenSSL::Crypto)
else()
    add_compile_definitions(NO_OPENSSL=1)
endif()

# live libraries
#===============================================================================
file(GLOB_RECURSE BasicUsageEnvironment_SOURCES
    "BasicUsageEnvironment/*.c"
    "BasicUsageEnvironment/*.cpp"
    "BasicUsageEnvironment/*.h"
    "BasicUsageEnvironment/*.hh"
)

file(GLOB_RECURSE groupsock_SOURCES
    "groupsock/*.c"
    "groupsock/*.cpp"
    "groupsock/*.h"
    "groupsock/*.hh"
)

file(GLOB_RECURSE liveMedia_SOURCES
    "liveMedia/*.c"
    "liveMedia/*.cpp"
    "liveMedia/*.h"
    "liveMedia/*.hh"
)

file(GLOB_RECURSE UsageEnvironment_SOURCES
    "UsageEnvironment/*.c"
    "UsageEnvironment/*.cpp"
    "UsageEnvironment/*.h"
    "UsageEnvironment/*.hh"
)

set(LIVE_SOURCES
    ${BasicUsageEnvironment_SOURCES}
    ${groupsock_SOURCES}
    ${liveMedia_SOURCES}
    ${UsageEnvironment_SOURCES}
)

add_library(live ${LIVE_SOURCES})

target_include_directories(live PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/BasicUsageEnvironment/include
    ${CMAKE_CURRENT_SOURCE_DIR}/groupsock/include
    ${CMAKE_CURRENT_SOURCE_DIR}/liveMedia/include
    ${CMAKE_CURRENT_SOURCE_DIR}/UsageEnvironment/include
    ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include
)

# myMediaServer libraries and executable
#===============================================================================
file(GLOB_RECURSE mms_SOURCES
    "myMediaServer/*.c"
    "myMediaServer/*.cpp"
    "myMediaServer/*.h"
    "myMediaServer/*.hh"
)

list(FILTER mms_SOURCES EXCLUDE REGEX "myMediaServer/main.cpp")

add_library(mms ${mms_SOURCES})

target_include_directories(live PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include
)

target_link_libraries(mms
    PUBLIC
        live
        pthread
)

add_executable(myMediaServer
    "myMediaServer/main.cpp"
)

target_link_libraries(myMediaServer mms)

add_custom_target(run
    COMMAND $<TARGET_FILE:myMediaServer>
    DEPENDS myMediaServer
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
