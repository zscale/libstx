# libxzero.pc.cmake

Name: libxzero
Description: Xzero HTTP library
Version: @LIBXZERO_VERSION@
# Requires:
# Conflicts:
Libs: -L@CMAKE_INSTALL_PREFIX@/lib -lxzero -lev -lpthread -pthread @POSIX_RT_LIBS@
Cflags: -I@CMAKE_INSTALL_PREFIX@/include -pthread @CFLAGS_RDYNAMIC@ @CXXFLAGS@
