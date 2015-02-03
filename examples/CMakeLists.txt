if(ENABLE_EXAMPLES)

  # add_executable(scheduler scheduler.cpp)
  # target_link_libraries(scheduler xzero)

  # add_executable(compress compress.cpp)
  # target_link_libraries(compress xzero)

  add_executable(echo-server echo-server.cpp)
  target_link_libraries(echo-server xzero)

  add_executable(echo-server2 echo-server2.cpp)
  target_link_libraries(echo-server2 xzero)

  # add_executable(http-hello1 http-hello1.cpp)
  # target_link_libraries(http-hello1 xzero)

  # add_executable(http-exceptions http-exceptions.cpp)
  # target_link_libraries(http-exceptions xzero)

  # add_executable(http-post-body http-post-body.cpp)
  # target_link_libraries(http-post-body xzero)

  # add_executable(http-staticfile http-staticfile.cpp)
  # target_link_libraries(http-staticfile xzero)

  # add_executable(http-threaded-blocking http-threaded-blocking.cpp)
  # target_link_libraries(http-threaded-blocking xzero)

  # add_executable(http-threaded-nbnio http-threaded-nbnio.cpp)
  # target_link_libraries(http-threaded-nbnio xzero)

  # add_executable(http-service http-service.cpp)
  # target_link_libraries(http-service xzero)

  # add_executable(threaded-executor threaded-executor.cpp)
  # target_link_libraries(threaded-executor xzero)

  # add_executable(hello_http hello_http.cpp)
  # target_link_libraries(hello_http xzero)

  # add_executable(hello_staticfile hello_staticfile.cpp)
  # target_link_libraries(hello_staticfile xzero)

  # add_executable(hello_service hello_service.cpp)
  # target_link_libraries(hello_service xzero)

endif()
