SET(welcome_sources
	main.cc
	welcomewindow.cc
	)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(welcome ${welcome_sources})
target_link_libraries(welcome ${GTKMM_LIBRARIES} ${X11_LIBRARIES} ${XSS_LIBRARIES} ${PANGOMM_LIBRARIES} ${DBUS_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_compile_definitions(welcome PRIVATE DATADIR=\"${CMAKE_INSTALL_FULL_DATADIR}/attingo/\")

configure_file(welcome.css.in welcome.css ESCAPE_QUOTES)

install(TARGETS welcome
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR}/${PROJECT_NAME})
#install(FILES welcome.1
#	DESTINATION ${CMAKE_INSTALL_FULL_MANDIR}/man1)
install(FILES welcome-bg.jpg ${CMAKE_CURRENT_BINARY_DIR}/welcome.css
	DESTINATION ${CMAKE_INSTALL_DATADIR}/attingo
	)
