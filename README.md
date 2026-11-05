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
We cross-compiled from linux to windows as a static binary. We don't want you to have the burden of carrying a fat SDL3.dll everywhere you put your sandsim.exe, and we don't want you to have the burden of even cross-compiling sandsim.exe.

SDL3's repository does not provide a static library in the releases tab, so we had to compile it ourselves. For your convenience, even though it is a bad practice, we embedded the libSDL3.a static library into the bin folder. We had to clone the SDL3 git repository, make a cross-compile toolchain file for cmake, and compile it with the static option, and the instructions that we followed to pull this off probably isn't going to work on everyone's machine.

If you want to cross-compile the Sand Simulaton yourself, install `mingw-w64-binutils`, `mingw-w64-headers`, and `mingw-w64-gcc` (or whatever the non-arch-linux equivalents are), then run `make win` to build the .exe with optimizations, or `make run-win` to also run it.

# How To Use
we will have sliders and stuff
