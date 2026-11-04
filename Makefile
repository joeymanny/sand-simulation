CXX := clang++
CXXFLAGS := --std=c++17 -xc++ -Wall -Wno-missing-prototypes -lSDL3

# Append ".exe" on windows
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

build/win/$(OUTPUT).exe: src/*.cpp src/headers/*.h
	mkdir -p build/win
	cp -f /usr/x86_64-w64-mingw32/bin/SDL3.dll .
	cp -f /usr/x86_64-w64-mingw32/bin/SDL3.dll build/win/
	$(CXX) $(CXXFLAGS) -O3 -s src/*.cpp -target x86_64-pc-win32-gnu -L/usr/x86_64-w64-mingw32/bin -I/usr/x86_64-w64-mingw32/include/ -o build/win/$(OUTPUT).exe

win: build/win/$(OUTPUT).exe
run-win: build/win/$(OUTPUT).exe
	@wine build/win/$(OUTPUT).exe

# Change "rm" to whatever comand you use on windows for removing files
clean:
	-@rm -rf build
	-@rm SDL3.dll
