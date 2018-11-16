project(testProgs)

file(GLOB TESTS test*.cpp)

if(${CMAKE_VERSION} VERSION_GREATER 3.5.2)
	list(FILTER TESTS EXCLUDE REGEX ".*testGSMStreamer.cpp$")
else()
	foreach(item ${TESTS})
		string(REGEX MATCH ".*testGSMStreamer.cpp$" item ${item})
		if(item)
			list(REMOVE_ITEM TESTS ${item})
		endif(item)
	endforeach()
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/BasicUsageEnvironment/include
	${CMAKE_SOURCE_DIR}/groupsock/include
	${CMAKE_SOURCE_DIR}/liveMedia/include
	${CMAKE_SOURCE_DIR}/UsageEnvironment/include
)

foreach(test IN LISTS TESTS)
	get_filename_component(tst ${test} NAME_WE)
	add_executable(${tst} ${test})
	target_link_libraries(${tst}
		liveMedia
		BasicUsageEnvironment
		groupsock
		UsageEnvironment
	)
endforeach(test)
