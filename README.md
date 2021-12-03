# Planet 3310 (NOKIA 3310 JAM 3)

This project was made for 2021's **[NOKIA 3310 JAM 3](https://itch.io/jam/nokiajam3)**.
It is a simple 2D shoot-em-up about getting the highest score possible. A download
for the game can be found over on **[itch.io](https://jrob774.itch.io/planet-3310)**,
the game is also playable in browser on the same page.

## Development

The game was developed in C and compiled using the **[TCC](https://bellard.org/tcc/)**
compiler. A port for web browsers was also developed using
**[emscripten](https://github.com/emscripten-core/emscripten)**.

## Building

### Windows

Run `build.bat win32` to build the application for Windows. The script is
setup to require *TCC* in order to build, (which is included within this
repository). Setting up the project to build with another C/C++ compiler would
also be relatively simple as it has no external dependencies.

### Web

Run `build.bat web` to build the application for the web. This requires *emscripten*
to be setup in order for the build to work. All external dependencies are provided
with the repo.

## License

The project's code is available under the **[MIT License](https://github.com/JROB774/nokiajam/blob/master/LICENSE)**.
