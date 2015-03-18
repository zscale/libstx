add_executable(dns-resolver dns-resolver.cc)
target_link_libraries(dns-resolver xzero-base)

add_executable(logging-simple logging-simple.cc)
target_link_libraries(logging-simple xzero-base)

add_executable(compress compress.cc)
target_link_libraries(compress xzero-base)

add_executable(echo-server echo-server.cc)
target_link_libraries(echo-server xzero-base)

add_executable(echo-server2 echo-server2.cc)
target_link_libraries(echo-server2 xzero-base)

add_executable(threaded-executor threaded-executor.cc)
target_link_libraries(threaded-executor xzero-base)

add_executable(udp-echo-server udp-echo-server.cc)
target_link_libraries(udp-echo-server xzero-base)

add_executable(udp-echo-client udp-echo-client.cc)
target_link_libraries(udp-echo-client xzero-base)
