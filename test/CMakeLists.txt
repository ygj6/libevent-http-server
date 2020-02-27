cmake_minimum_required(VERSION 3.5)
project(pipeline-client C)

add_executable(pipeline-client pipeline-client.c)

set(CURL_ROOT "$ENV{HOME}/libcurl-vc15-x64-release-dll-ipv6-sspi-winssl")
include_directories("${CURL_ROOT}/include")
target_link_libraries(pipeline-client PRIVATE ${CURL_ROOT}/lib/libcurl.lib)

enable_testing()
# ctest -C Debug -V
add_test(NAME pipeline-client
        CONFIGURATIONS Debug
        COMMAND pipeline-client 100 http://127.0.0.1:6790/)
string(REPLACE ";" "\\;" ESCAPED_PATH "$ENV{PATH}")
set_tests_properties(pipeline-client PROPERTIES ENVIRONMENT "PATH=${CURL_ROOT}/lib\\;${CURL_ROOT}/bin\\;${ESCAPED_PATH}")
