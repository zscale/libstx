# Cortex C++ Library Collection

- github: https://github.com/cortex/libcortex
- travis-ci: https://travis-ci.org/cortex/libcortex

This is a collection of modern C++ system libraries.

## Installation Requirements

- OpenSSL (libssl-dev)
- gcc >= 4.8.0 (for building only, clang >= 3.4 with libc++)
- cmake (for building only)
- pkg-config (for building only)
- gtest (optional, for unit testing)

### Install From Source: Ubuntu

```
# Build gtest dependency
apt-get install gtest-dev
cd /usr/src/gtest
cmake .
make
cp *.a /usr/local/lib/
```

```
apt-get install git clang++ cmake pkg-config libssl-dev libbz2-dev
git clone git@github.com:cortex/libcortex.git
cd libcortex
make debug
```

## Contributors

- Christian Parpart (https://github.com/christianparpart)
- Paul Asmuth (https://github.com/paulasmuth)

LICENSE
-------

```
Copyright (c) 2009-2014 Christian Parpart <trapni@gmail.com>
Copyright (c) 2011-2015 Paul Asmuth

libcortex is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

libcortex is distributed in the hope that it will be useful,but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with libcortex. If not, see <http://www.gnu.org/licenses/>.
