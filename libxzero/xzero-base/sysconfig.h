// This file is part of the "libxzero" project
//   (c) 2009-2015 Christian Parpart <https://github.com/christianparpart>
//   (c) 2014-2015 Paul Asmuth <https://github.com/paulasmuth>
//
// libxzero is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License v3.0.
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef xzero_sysconfig_h
#define xzero_sysconfig_h (1)

//#include <base/sysconfig.h>

/* #undef LIBXZERO_VERSION */

// --------------------------------------------------------------------------
// feature tests

// Build with inotify support
/* #undef XZERO_ENABLE_INOTIFY */

/* #undef ENABLE_ACCEPT4 */
/* #undef ENABLE_PIPE2 */

/* #undef ENABLE_MULTI_ACCEPT */

/* #undef ENABLE_PCRE */

/* #undef ENABLE_INOTIFY */

// Enable support for TCP_DEFER_ACCEPT
/* #undef ENABLE_TCP_DEFER_ACCEPT */

// Try to open temporary files with O_TMPFILE flag before falling back
// to the standard behaviour.
/* #undef XZERO_ENABLE_O_TMPFILE */

/* #undef XZERO_ENABLE_NOEXCEPT */

// Builds with support for opportunistic write() calls to client sockets
/* #undef XZERO_OPPORTUNISTIC_WRITE */

// --------------------------------------------------------------------------
// header tests

#define HAVE_SYS_INOTIFY_H
#define HAVE_SYS_SENDFILE_H
#define HAVE_SYS_RESOURCE_H
/* #undef HAVE_SYS_LIMITS_H */
#define HAVE_SYS_MMAN_H
#define HAVE_SYSLOG_H
#define HAVE_DLFCN_H
#define HAVE_EXECINFO_H
#define HAVE_PWD_H
#define HAVE_UNISTD_H
#define HAVE_PTHREAD_H

/* #undef HAVE_NETDB_H */
/* #undef HAVE_AIO_H */
/* #undef HAVE_LIBAIO_H */
#define HAVE_ZLIB_H
/* #undef HAVE_BZLIB_H */
/* #undef HAVE_GNUTLS_H */
/* #undef HAVE_LUA_H */
/* #undef HAVE_PCRE_H */
/* #undef HAVE_SYS_UTSNAME_H */
/* #undef HAVE_SECURITY_PAM_APPL_H */

// --------------------------------------------------------------------------
// functional tests

#define HAVE_INOTIFY_INIT1
/* #undef HAVE_CHROOT */
#define HAVE_PATHCONF
#define HAVE_SENDFILE
#define HAVE_POSIX_FADVISE
#define HAVE_READAHEAD
#define HAVE_PREAD
#define HAVE_SYSCONF
#define HAVE_PATHCONF
#define HAVE_ACCEPT4
#define HAVE_PIPE2
#define HAVE_DUP2
#define HAVE_FORK
#define HAVE_BACKTRACE
#define HAVE_CLOCK_GETTIME
#define HAVE_PTHREAD_SETNAME_NP
#define HAVE_PTHREAD_SETAFFINITY_NP

#endif
