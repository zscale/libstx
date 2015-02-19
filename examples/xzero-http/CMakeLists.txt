add_executable(http-hello1 http-hello1.cpp)
target_link_libraries(http-hello1 xzero-http)

add_executable(http-exceptions http-exceptions.cpp)
target_link_libraries(http-exceptions xzero-http)

add_executable(http-post-body http-post-body.cpp)
target_link_libraries(http-post-body xzero-http)

add_executable(http-service http-service.cpp)
target_link_libraries(http-service xzero-http)

add_executable(http-staticfile http-staticfile.cpp)
target_link_libraries(http-staticfile xzero-http)

add_executable(http-threaded-nbnio http-threaded-nbnio.cpp)
target_link_libraries(http-threaded-nbnio xzero-http)
