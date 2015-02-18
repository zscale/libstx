# libxzero.pc.cmake

Name: libxzero
Description: Xzero Base library
Version: @XZERO_VERSION@
# Requires:
# Conflicts:
Libs: -L@CMAKE_INSTALL_PREFIX@/lib -lxzero-base -lev -lpthread -pthread @POSIX_RT_LIBS@
Cflags: -I@CMAKE_INSTALL_PREFIX@/include -pthread @CFLAGS_RDYNAMIC@ @CXXFLAGS@
