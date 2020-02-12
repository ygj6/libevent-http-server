cmake_minimum_required(VERSION 3.5)
project(libevent-http-server CXX)

add_executable(main main.cpp)

set(VCPKG_ROOT "C:/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_INSTALLED "${VCPKG_ROOT}/installed/x64-windows")
include_directories("${VCPKG_INSTALLED}/include")

list(APPEND CMAKE_PREFIX_PATH
    "${VCPKG_INSTALLED}/share/libevent"
    "${VCPKG_INSTALLED}/share/glog"
    "${VCPKG_INSTALLED}/share/gflags"
)

find_package(Libevent CONFIG REQUIRED)
target_link_libraries(main PRIVATE event_shared event_core_shared event_extra_shared)

find_package(glog CONFIG REQUIRED)
target_link_libraries(main PRIVATE glog::glog)
