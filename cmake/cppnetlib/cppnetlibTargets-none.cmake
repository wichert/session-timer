#----------------------------------------------------------------
# Generated CMake target import file for configuration "None".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cppnetlib-uri" for configuration "None"
set_property(TARGET cppnetlib-uri APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(cppnetlib-uri PROPERTIES
  IMPORTED_LOCATION_NONE "/usr/lib/x86_64-linux-gnu/libcppnetlib-uri.so.0.11.1"
  IMPORTED_SONAME_NONE "libcppnetlib-uri.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS cppnetlib-uri )
list(APPEND _IMPORT_CHECK_FILES_FOR_cppnetlib-uri "/usr/lib/x86_64-linux-gnu/libcppnetlib-uri.so.0.11.1" )

# Import target "cppnetlib-server-parsers" for configuration "None"
set_property(TARGET cppnetlib-server-parsers APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(cppnetlib-server-parsers PROPERTIES
  IMPORTED_LOCATION_NONE "/usr/lib/x86_64-linux-gnu/libcppnetlib-server-parsers.so.0.11.1"
  IMPORTED_SONAME_NONE "libcppnetlib-server-parsers.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS cppnetlib-server-parsers )
list(APPEND _IMPORT_CHECK_FILES_FOR_cppnetlib-server-parsers "/usr/lib/x86_64-linux-gnu/libcppnetlib-server-parsers.so.0.11.1" )

# Import target "cppnetlib-client-connections" for configuration "None"
set_property(TARGET cppnetlib-client-connections APPEND PROPERTY IMPORTED_CONFIGURATIONS NONE)
set_target_properties(cppnetlib-client-connections PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_NONE "/usr/lib/x86_64-linux-gnu/libssl.so;/usr/lib/x86_64-linux-gnu/libcrypto.so"
  IMPORTED_LOCATION_NONE "/usr/lib/x86_64-linux-gnu/libcppnetlib-client-connections.so.0.11.1"
  IMPORTED_SONAME_NONE "libcppnetlib-client-connections.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS cppnetlib-client-connections )
list(APPEND _IMPORT_CHECK_FILES_FOR_cppnetlib-client-connections "/usr/lib/x86_64-linux-gnu/libcppnetlib-client-connections.so.0.11.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
