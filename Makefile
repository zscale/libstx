
help:
	@echo "make help     Prints this help"
	@echo "make debug    Generates debug build"
	@echo "make relase   Generates release build"
	@echo "make clean    Cleans all build files"

debug:
	@mkdir -p build/debug
	@cd build/debug && cmake ../.. \
		-DCMAKE_BUILD_TYPE="debug" \
		-DCMAKE_CXX_FLAGS_DEBUG="-O0 -g3 -fsanitize=address" \
		-DENABLE_EXAMPLES=yes \
		-DENABLE_TESTS=yes \
		-DCMAKE_INSTALL_PREFIX="${PWD}/build/target/debug"
	@cd build/debug && make

test:
	@cd build/debug && ./test/libxzero-test

release:
	mkdir -p build/release
	cd build/release && cmake ../.. \
		-DCMAKE_BUILD_TYPE="release" \
		-DCMAKE_CXX_FLAGS_RELEASE="-O3 -g -march=native" \
		-DENABLE_EXAMPLES=yes \
		-DENABLE_TESTS=yes \
		-DCMAKE_INSTALL_PREFIX="/usr"
	@cd build/release && make

clean:
	rm -rf build/debug
	rm -rf build/release

.PHONY: help debug test release clean
