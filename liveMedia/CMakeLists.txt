project(liveMedia)

file(GLOB SOURCES *.cpp *.c)
file(GLOB PUBLIC_HEADERS include/*.hh include/*.h)

include_directories(
	include
	${CMAKE_SOURCE_DIR}/BasicUsageEnvironment/include
	${CMAKE_SOURCE_DIR}/groupsock/include
	${CMAKE_SOURCE_DIR}/UsageEnvironment/include
)

add_library(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME}
	PROPERTIES
		PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

INSTALL(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include/live
)