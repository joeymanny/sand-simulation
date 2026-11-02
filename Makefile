CXX := clang++
CXXFLAGS := --std=c++17 -xc++ -Wall -Wno-missing-prototypes -lSDL3

# Append ".exe" on windows
OUTPUT := sandsim

build/debug/$(OUTPUT): src/*.cpp # Default target, builds the binary with debug symbols to build/debug
	mkdir -p build/debug
	$(CXX) $(CXXFLAGS) -g src/*.cpp -o build/debug/$(OUTPUT)

debug: build/debug/$(OUTPUT) # Convenient alias (make debug)
run: build/debug/$(OUTPUT) # Runs the program easily, building if needed (make run)
	@build/debug/$(OUTPUT)

build/release/$(OUTPUT): src/*.cpp # Builds the binary with optimizations and stripped symbols
	mkdir -p build/release
	$(CXX) $(CXXFLAGS) -O3 -s src/*.cpp -o build/release/$(OUTPUT)

release: build/release/$(OUTPUT) # Convenient alias (make release)
run-release: build/release/$(OUTPUT) # Runs the optimized program easily, building if needed (make run-release)
	@build/release/$(OUTPUT)

# Change "rm" to whatever comand you use on windows for removing files
clean:
	-rm -rf build
