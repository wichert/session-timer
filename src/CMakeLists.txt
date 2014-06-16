include(GNUInstallDirs)

add_custom_command(
	OUTPUT xfce-sessionmanager-proxy.hh xfce-sessionmanager-adaptor.hh
	COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/xfce-sessionmanager.xml
		--proxy=xfce-sessionmanager-proxy.hh
		--adaptor=xfce-sessionmanager-adaptor.hh
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/xfce-sessionmanager.xml
	)

SET(countdown_sources
	countwindow.cc
	main.cc
	xfce-sessionmanager-adaptor.hh
	xfce-sessionmanager-proxy.hh
	)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(countdown ${countdown_sources})
target_link_libraries(countdown ${GTKMM_LIBRARIES} ${PANGOMM_LIBRARIES} ${DBUS_LIBRARIES})
