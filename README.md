# Xzero C++ Library Collection

[ ![Build Status - Travis-CI](https://secure.travis-ci.org/xzero/xzero.png) ](http://travis-ci.org/xzero/xzero)

- github: https://github.com/xzero/xzero
- travis-ci: https://travis-ci.org/xzero/xzero

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

## Uses

- TODO

LICENSE
-------

```
Copyright (c) 2009-2014 Christian Parpart <trapni@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
