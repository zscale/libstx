all: build

build:
	mkdir -p build
	(cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ../ && ninja)

test: build
	@find build -name "test-*" -exec ./{} \;

clean:
	rm -rf build


.PHONY: clean build test
