all: build

build:
	mkdir -p build/devel
	cd build/devel && \
	cmake -DCMAKE_BUILD_TYPE=Debug ../.. && \
	make

clean:
	rm -rf build

.PHONY: build test clean
