# Unnamed Sand Simulation
it simulates sand

## Running On Linux
In order to build (and probably also run), you need SDL3 installed. On linux, install it through whatever package manager you use. For Pacman, the package is `sdl3`. For APT (but not on linux mint for some reason), the package is `libsdl3-dev`. For DNF, the package is `SDL3-devel`.

Run `make` to build the project

Run `make run` to build AND run the project in one command

Run `make release` to build the project with optimizations

Run `make run-release` to build AND run the project with optimizations in one command

## Running On Windows
Good luck because we can't bother learning CMake at this time... [Maybe this would be helpful](https://www.lazyfoo.net/tutorials/SDL3/01-hello-sdl3/windows/index.php).

If you don't want to deal with building on your own system, I have provided a pre-compiled binary in the Releases tab. (thats what I would say once this project is done)

### How did we compile it?
We cross-compiled from linux to windows.

On arch-based distros (look for a similar package on non-arch distros), install `mingw-w64-binutils`, `mingw-w64-headers`, and `wine`. Download the [SDL3 library and header files](https://github.com/libsdl-org/SDL/releases/download/release-3.4.20/SDL3-devel-3.4.20-mingw.zip), extract the archive, and copy the folders in `x86_64-w64-mingw32` to `/usr/x86_64-w64-mingw32`. Lastly, run `make win` to build the windows binary with optimizations, or run `make run-win` to also run the program.

# How To Use
we will have sliders and stuff
