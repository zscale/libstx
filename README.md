# libxzero C++ HTTP Library

[ ![Build status - Travis-ci](https://secure.travis-ci.org/xzero/libxzero.png) ](http://travis-ci.org/xzero/libxzero)

- official website: http://xzero.io/
- github: https://github.com/xzero/libxzero
- travis-ci: https://travis-ci.org/xzero/libxzero

`libxzero` is a thin low-latency, scalable, and embeddable HTTP server library
written in modern C++.

### Feature Highlights

* Composable API
- HTTP/1.1, including chunked trailer and pipelining support
* Generic core HTTP API with support for: HTTP (HTTPS, FCGI, HTTP/2, ...)
- Client-cache aware and partial static file serving
- zero-copy networking optimizations
- Unit Tests

## Installation Requirements

- gcc >= 4.8.0 (for building only, CLANG >= 3.4 with libc++ is also supported)
- cmake (for building only)
- gtest (optional, for unit testing)

### Building from Source on Ubuntu 14.04:

```sh
# Installs required dependencies
sudo apt-get install make cmake pkg-config git gcc-4.8 g++-4.8 \
    zlib1g-dev libpcre3-dev

# If you want to built the tests, you must install libgtest-dev
# and then built it yourself
sudo apt-get install libgtest-dev
cd /usr/src/gtest && sudo cmake . && sudo make && \
     sudo cp -vpi libgtest*.a /usr/local/lib/; cd -

# Clone the repository
git clone git://github.com/xzero/libxzero.git && cd libxzero

# Now run cmake to bootstrap the build
cmake .

# Now compiling should just work
make && sudo make install

# Try out an example
./examples/http-hello1
```

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
