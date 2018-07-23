project(testProgs)

file(GLOB TESTS test*.cpp)
list(FILTER TESTS EXCLUDE REGEX ".*testGSMStreamer.cpp$")

include_directories(
	${CMAKE_SOURCE_DIR}/BasicUsageEnvironment/include
	${CMAKE_SOURCE_DIR}/groupsock/include
	${CMAKE_SOURCE_DIR}/liveMedia/include
	${CMAKE_SOURCE_DIR}/UsageEnvironment/include
)

foreach(test IN LISTS TESTS)
	#message("test ${test}")
	get_filename_component(tst ${test} NAME_WE)
	add_executable(${tst} ${test})
	target_link_libraries(${tst}
		liveMedia
		BasicUsageEnvironment
		groupsock
		UsageEnvironment
	)
endforeach(test)
