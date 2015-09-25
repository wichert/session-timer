SET(manager_sources
	main.cc
	signalfd.cc
	robustchild.cc
	poller.cc
	state.cc
	)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DLIBEXEC_PATH="${CMAKE_INSTALL_FULL_LIBEXECDIR}/${PROJECT_NAME}")
add_definitions(-DXSESSION_PATH="/etc/X11/Xsession")

find_package(Threads)

add_executable(kiosk-manager ${manager_sources})
target_link_libraries(kiosk-manager
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_LOG_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
	${CURLPP_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT})

install(TARGETS kiosk-manager
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
# install(FILES kiosk-manager.1
# 	DESTINATION ${CMAKE_INSTALL_FULL_MANDIR}/man1)
