project(live555ProxyServer)

list(APPEND SOURCES
	live555ProxyServer.cpp
)

include_directories(
	${CMAKE_SOURCE_DIR}/BasicUsageEnvironment/include
	${CMAKE_SOURCE_DIR}/groupsock/include
	${CMAKE_SOURCE_DIR}/liveMedia/include
	${CMAKE_SOURCE_DIR}/UsageEnvironment/include
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
	liveMedia
	BasicUsageEnvironment
	groupsock
	UsageEnvironment
)
