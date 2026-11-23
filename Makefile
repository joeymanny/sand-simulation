# THIS MAKEFILE WILL LIKELY NOT WORK UNDER WINDOWS!!

# Compiler and compiler flags
CXX := clang++
CXXFLAGS := --std=c++17 -xc++ -Wall -Wno-missing-prototypes -lSDL3
CXXFLAGS_WIN := --std=c++17 -pthread

# Output program
OUTPUT := sandsim

build/debug/$(OUTPUT): src/*.cpp src/headers/*.h # Default target, builds the binary with debug symbols to build/debug
	mkdir -p build/debug
	$(CXX) $(CXXFLAGS) -g src/*.cpp -o build/debug/$(OUTPUT)

debug: build/debug/$(OUTPUT) # Convenient alias (make debug)
run: build/debug/$(OUTPUT) # Runs the program easily, building if needed (make run)
	@build/debug/$(OUTPUT)

build/release/$(OUTPUT): src/*.cpp src/headers/*.h # Builds the binary with optimizations and stripped symbols
	mkdir -p build/release
	$(CXX) $(CXXFLAGS) -O3 -s src/*.cpp -o build/release/$(OUTPUT)

release: build/release/$(OUTPUT) # Convenient alias (make release)
run-release: build/release/$(OUTPUT) # Runs the optimized program easily, building if needed (make run-release)
	@build/release/$(OUTPUT)

# Static libraries for cross compilation from linux to windows
STATICS := libgdi32.a libhid.a libimm32.a libole32.a liboleaut32.a libsetupapi.a libuuid.a libversion.a libwinmm.a
STATIC_PATHS := $(foreach static,$(STATICS),/usr/x86_64-w64-mingw32/lib/$(static))

build/win/$(OUTPUT).exe: src/*.cpp src/headers/*.h
	mkdir -p build/win
	$(CXX) $(CXXFLAGS_WIN) -target x86_64-pc-win32-gnu -mwindows -O3 -s src/*.cpp -static-libstdc++ -static bin/libSDL3.a $(STATIC_PATHS) \
		-L/usr/x86_64-w64-mingw32/lib -I/usr/x86_64-w64-mingw32/include/ -o build/win/$(OUTPUT).exe

win: build/win/$(OUTPUT).exe # Convenient alias (make win)
run-win: build/win/$(OUTPUT).exe # Runs the cross-compiled program through wine, building if needed (make run-win)
	@wine build/win/$(OUTPUT).exe

clean:
	-@rm -rf build
