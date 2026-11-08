# Tread Marks

Tread Marks is a battle tank racing game by Longbow Digital Arts: you race
tanks over deformable voxel terrain while shooting at each other. The source
was released under the GNU GPL v3 (see [Resources/LICENSE](Resources/LICENSE)).

This fork updates version 1.7.0, whose official build was a 32-bit Windows
program (the original notes mention only a macOS build with broken textures
and a headless Linux server), so that the full game builds and runs on
current 64-bit systems.

## Status

**The game is up and running.** On 64-bit Linux it plays from the menu to a
full race, with terrain, particles, positional sound and music working, and
as far as I can tell it behaves like the original. The 64-bit Windows build
compiles and its launcher and dedicated server run; the Windows game client
is the next thing to check.

The game was played by hand on 64-bit Linux (Linux Mint 22, GCC 13, an Intel
GPU on Mesa): menus, races, terrain, particles, sound effects and music all
work. On 64-bit Windows the code was cross-compiled with MinGW-w64, and the
dedicated server and the launcher ran under Wine; the game client itself and
Visual Studio builds haven't been tried on Windows. 32-bit Windows, the
original target, keeps its code paths but wasn't retested, and macOS wasn't
tested at all.

## What changed

Each item is a separate commit in the history.

- **Build system.** CMake finds SFML 2.5+ through its package config, falling
  back to the old `FindSFML` module, and accepts either Qt 5 or Qt 6. The
  code builds as C++17 and uses `std::filesystem`. On Linux, everything
  installs into the install prefix rather than the filesystem root. GCC and
  Clang get `-fno-strict-aliasing -fwrapv`, because the code was written for
  MSVC and type-puns floats through integer pointers.
- **64-bit data layouts.** `long` is 64 bits on Linux and macOS, so every
  structure read from disk or the network through it had the wrong layout.
  BMP headers, IFF/LWO chunks, network floats, the packet encryption header,
  32-bit pixel buffers and the game clock now use fixed-width types, and
  pointers are aligned through `uintptr_t`.
- **Windows-style paths.** Code and data refer to files with `\` separators
  and with a case that doesn't always match the disk (`maps/` for `Maps/`).
  On POSIX systems, opening a file translates separators and matches each
  path component's case against the disk. Wildcard searches are
  case-insensitive and return entries in NTFS order, which decides the order
  tanks and maps are listed in.
- **Data directories.** User data (settings, logs, scores) lives in
  `$XDG_DATA_HOME/Tread Marks/`, normally `~/.local/share/Tread Marks/`. At
  startup the game changes to its executable's directory when its data isn't
  in the current one, so it can be launched from anywhere.
- **OpenGL.** `glewInit()` now runs after the GL context exists. Before, it
  ran before any window was created and left every extension unavailable.
- **Locale.** Qt switches the C library to the user's locale on Unix. Under a
  decimal-comma locale (pt_BR, de_DE, ...) `atof("0.5")` returned 0, which
  broke menu effects, the intro, terrain quality and physics, and got saved
  to the settings file. Numbers are now parsed in the C locale.
- **Positional audio.** 3D sound positions are now passed to OpenAL, and the
  listener's orientation is computed from the camera angles. Z is flipped
  because the game's world is left-handed and OpenAL's is right-handed.
  Interface sounds and music stay relative to the listener. Before, every
  sound panned as if it came from the world origin, and the menu music was
  inaudible.
- **Undefined behaviour.** Fixed an unsequenced `x + (++x)` in the colour
  quantizer and a `sprintf` into its own source buffer.
- **64-bit Windows.** The x86 inline assembly is only used with 32-bit Visual
  C++, and a C version rounds the same way elsewhere. Sockets use the native
  `SOCKET` type, which is 64 bits on Win64. The launcher's dialog procedure
  returns `INT_PTR`, and `ShellExecute` results are no longer truncated to
  `int`. The code also builds with MinGW-w64.

## Building on Linux

### Requirements

- CMake 3.10 or newer, and a C++17 compiler (GCC 8+ or Clang 7+)
- SFML 2.5 or 2.6 (SFML 3 isn't supported)
- GLEW, OpenGL and zlib
- Qt 5 or Qt 6 (Widgets)

On Debian, Ubuntu or Linux Mint:

```bash
sudo apt install build-essential cmake libsfml-dev libglew-dev zlib1g-dev qtbase5-dev
```

`qt6-base-dev` works instead of `qtbase5-dev`.

On Fedora the equivalent packages are `gcc-c++ cmake SFML-devel glew-devel
zlib-devel qt5-qtbase-devel`. Other distributions need the same libraries
under their own package names.

### Build and run

```bash
cmake -S . -B build
cmake --build build -j
cmake --install build
./out/TM
```

The install step copies the game and its data into `out/`. You can move that
directory anywhere and start `TM` from any working directory.

Useful CMake options:

| Option | Default | Effect |
|---|---|---|
| `-DBUILD_HEADLESS=true` | `false` | Builds only the dedicated server, without graphics, sound or Qt. Needs only zlib. |
| `-DBUILD_MASTER=ON` | `OFF` | Also builds the master server (`TMMaster`). |
| `-DCMAKE_BUILD_TYPE=Release` | `RelWithDebInfo` | Picks the build type. |
| `-DCMAKE_INSTALL_PREFIX=...` | `out/` | Picks the install directory. |

## Building on Windows

### Requirements

- Visual Studio 2017 (15.7) or newer, for C++17 `std::filesystem`
- CMake 3.10 or newer
- [SFML](https://www.sfml-dev.org/download.php) 2.x
- [GLEW](http://glew.sourceforge.net/)
- [zlib](https://zlib.net/)
- [Qt](https://www.qt.io/download-open-source/) 5 or 6
- Optionally, NSIS to build the installer

All libraries must match the architecture you build for: all 32-bit, or all
64-bit.

### Configure and build

1. Open the CMake GUI. Set the source directory to this folder and the build
   directory to, for example, `build`.
2. Press **Configure** and pick your Visual Studio version. For the platform,
   pick **Win32** for a 32-bit build or **x64** for a 64-bit build.
3. If CMake can't find a library, set its root directory and press
   **Configure** again:
   - `PATH_GLEW`, e.g. `C:\3rdParty\glew-2.1.0`
   - `PATH_SFML`, e.g. `C:\3rdParty\SFML-2.6.1`
   - `PATH_ZLIB`, e.g. `C:\3rdParty\zlib-1.3`
   - `PATH_QT`, e.g. `C:\Qt\5.15.2\msvc2019_64`
   - `PATH_UCRT` (optional), the Universal CRT redistributables to ship with
     the installer
4. Press **Generate** and open `build\TreadMarks.sln`.

The same steps from the command line:

```bat
cmake -S . -B build -A x64 -DPATH_SFML=C:\3rdParty\SFML-2.6.1 -DPATH_GLEW=C:\3rdParty\glew-2.1.0 -DPATH_ZLIB=C:\3rdParty\zlib-1.3 -DPATH_QT=C:\Qt\5.15.2\msvc2019_64
cmake --build build --config Release
```

Use `-A Win32` instead of `-A x64` for a 32-bit build.

### Projects and targets

| Target | Purpose |
|---|---|
| `TM` | The game. |
| `TreadMarks` | The launcher, which picks graphics settings and starts `TM.exe`. |
| `FIXUP` | Copies the required DLLs next to `TM.exe`. To run or debug from Visual Studio, also set the working directory to `Resources`. |
| `INSTALL` | Installs the game and its data into `CMAKE_INSTALL_PREFIX`. |
| `PACKAGE` | Builds the NSIS installer. 64-bit builds install into the 64-bit Program Files. |
| `SIGN` | Signs the installer, if you set the `CERT` and `CERT_PASS` CMake variables. |

### zlib from GnuWin32

The GnuWin32 build of zlib has a
[long-standing bug](https://sourceforge.net/p/gnuwin32/bugs/169/). To compile
against it, edit `include/zconf.h` and change `#if 1 /* HAVE_UNISTD_H` to
`#ifdef HAVE_UNISTD_H`.

## Playing

- Settings changed in game are saved to `reg.ini` in the user data directory
  and take precedence over `config.cfg`. Delete `reg.ini` to go back to the
  defaults.
- Keys **1** to **0** set the terrain detail from 0.1 to 1.0 during a race.
- The game log is `tank.log`. On Linux it's in the user data directory. On
  Windows, settings go in `%APPDATA%\Longbow Digital Arts\Tread Marks\`,
  while the log and scores go in
  `%PROGRAMDATA%\Longbow Digital Arts\Tread Marks\`.
- Run a dedicated server with `TM dedicated.cfg`.
  [Resources/Server Commands.txt](Resources/Server%20Commands.txt) lists the
  console commands.

[BUILD.txt](BUILD.txt) keeps the original build notes.

## License

Tread Marks is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 3 or later. See
[Resources/LICENSE](Resources/LICENSE).

Originally created by Seumas McNally and Longbow Digital Arts.
