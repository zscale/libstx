add_executable(http-hello1 http-hello1.cc)
target_link_libraries(http-hello1 xzero-http)

add_executable(http-exceptions http-exceptions.cc)
target_link_libraries(http-exceptions xzero-http)

add_executable(http-post-body http-post-body.cc)
target_link_libraries(http-post-body xzero-http)

add_executable(http-service http-service.cc)
target_link_libraries(http-service xzero-http)

add_executable(http-staticfile http-staticfile.cc)
target_link_libraries(http-staticfile xzero-http)

add_executable(http-threaded-nbnio http-threaded-nbnio.cc)
target_link_libraries(http-threaded-nbnio xzero-http)
