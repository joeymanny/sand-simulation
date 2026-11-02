# Unnamed Sand Simulation
it simulates sand

## Running On Linux
In order to build (and probably also run), you need SDL3 installed. On linux, install it through whatever package manager you use. For Pacman, the package is `sdl3`. For APT, the package is `libsdl3-dev`. For DNF, the package is `SDL3-devel`. There is a good chance it is pre-installed.

Run `make` to build the project

Run `make run` to build AND run the project in one command

Run `make release` to build the project with optimizations

Run `make run-release` to build AND run the project with optimizations in one command

## Running On Windows
Good luck because we can't bother learning CMake at this time... [Maybe this would be helpful](https://www.lazyfoo.net/tutorials/SDL3/01-hello-sdl3/windows/index.php).
