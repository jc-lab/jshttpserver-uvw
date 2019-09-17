cmake_minimum_required(VERSION 3.14)
project(jshttpserver_uvw)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(uvw)
include_directories(uvw/src)

add_definitions(-D_WIN32)

include_directories(libuv-prebuilt-winx86/include http-parser/)
link_directories(jshttpserver_uvw libuv-prebuilt-winx86/msvc_lib/v140/Win32)


include_directories(include)
set(JSHTTPSERVER_INC include/jshttpserver/server.hpp include/jshttpserver/client.hpp include/jshttpserver/http_events.hpp include/jshttpserver/http_request.hpp)
set(JSHTTPSERVER_SRC src/server.cpp src/client.cpp src/http_events.cpp)
add_library(jshttpserver_uvw example/main.cpp http-parser/http_parser.c ${JSHTTPSERVER_SRC})

add_subdirectory(example)
