SET(welcome_sources
	main.cc
	welcomewindow.cc
	)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(kiosk-welcome ${welcome_sources})
target_link_libraries(kiosk-welcome ${GTKMM_LIBRARIES} ${X11_LIBRARIES} ${XSS_LIBRARIES} ${PANGOMM_LIBRARIES} ${DBUS_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_compile_definitions(kiosk-welcome PRIVATE DATADIR=\"${CMAKE_INSTALL_FULL_DATADIR}/kiosk-welcome/\")

install(TARGETS kiosk-welcome
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR}/${PROJECT_NAME})
#install(FILES kiosk-welcome.1
#	DESTINATION ${CMAKE_INSTALL_FULL_MANDIR}/man1)
install(FILES welcome.css
	DESTINATION ${CMAKE_INSTALL_DATADIR}/kiosk-welcome
	)
