# Xzero C++ Library Collection

[ ![Build Status - Travis-CI](https://secure.travis-ci.org/xzero/libxzero.png) ](http://travis-ci.org/xzero/libxzero)

- github: https://github.com/xzero/libxzero
- travis-ci: https://travis-ci.org/xzero/libxzero

This is a collection of C++ system libraries.

## Installation Requirements

- OpenSSL (libssl-dev)
- gcc >= 4.8.0 (for building only, clang >= 3.4 with libc++)
- cmake (for building only)
- pkg-config
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
git clone git@github.com:xzero/libxzero.git
cd libxzero
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

libxzero is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

libxzero is distributed in the hope that it will be useful,but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with libxzero. If not, see <http://www.gnu.org/licenses/>.
