project(countdown)
cmake_minimum_required(VERSION 2.8)
enable_testing()

set(PACKGE countdown)
set(VERSION 1.0.0)
set(COPYRIGHT_HOLDER Attingo Services B.V.)
set(PACKAGE_BUGREPORT wichert@simplon.biz)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIBMM glibmm-2.4 REQUIRED)
include_directories(SYSTEM ${GLIBMM_INCLUDE_DIRS})
link_directories(${GLIBMM_LIBRARY_DIRS})

pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
include_directories(SYSTEM ${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

pkg_check_modules(PANGOMM pangomm-1.4 REQUIRED)
include_directories(SYSTEM ${PANGOMM_INCLUDE_DIRS})
link_directories(${PANGOMM_LIBRARY_DIRS})

pkg_check_modules(GDK_PIXBUF_XLIB gdk-pixbuf-xlib-2.0 REQUIRED)
include_directories(SYSTEM ${GDK_PIXBUF_XLIB_INCLUDE_DIRS})
link_directories(${GDK_PIXBUF_XLIB_LIBRARY_DIRS})

pkg_check_modules(DBUS dbus-c++-glib-1 REQUIRED)
include_directories(SYSTEM ${DBUS_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})

pkg_check_modules(X11 x11 REQUIRED)
include_directories(SYSTEM ${X11_INCLUDE_DIRS})
link_directories(${X11_LIBRARY_DIRS})

pkg_check_modules(XSS xscrnsaver REQUIRED)
include_directories(SYSTEM ${XSS_INCLUDE_DIRS})
link_directories(${XSS_LIBRARY_DIRS})

set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS program_options log system thread log)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_definitions(-DBOOST_ALL_DYN_LINK)

add_subdirectory(src)
