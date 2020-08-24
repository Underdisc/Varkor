# Varkor

Varkor is a custom game engine project that will also do other cool things that aren't necessarily for games.

## Building Varkor

*Disclaimer*: This is how I build the project. Feel free to use your own method if you are inclined. Additionally, these instructions only cover how to create release executables. I will add instructions for debug executables once I arrive at that point.

*Once You Build*: After you build the project, you'll find `varkor.exe` in `{repo_root}/working/`. This directory contains all the files that varkor may access at runtime. This is why the Varkor binary is placed here.

At the moment, Varkor only builds under Windows, but will be expanded in the near future for building under Linux as well. On Windows, Varkor can be built under MinGW32 and MSVC. To build Varkor, you are going to want a build of CMake and Ninja.

- CMake: https://cmake.org/download/
- Ninja: https://github.com/ninja-build/ninja/releases

Once you have those, make sure *ninja.exe* and *cmake.exe* are in your path.

### MSVC

Create a build directory at the root of your local copy of the repo.

```
mkdir build
```

Within the build directory, I like to have multiple directories for different build types under MSVC. Right now I have `msvc32/` and `msvc64/` in my build directory.

```
mkdir msvc32 msvc64
cd msvc32
```

This will cover MSVC 32bit, but the process for MSVC 32bit is exactly the same as MSVC 64bit except for one detail. The first step is to enter the cmd shell that comes with Windows. I use the [Mintty](https://mintty.github.io/) terminal that comes with Cygwin and I get to the cmd shell by typing `cmd` and the shell starts running within my terminal. You can also choose to run cmd with the terminal that comes with Windows by typing `cmd` and pressing enter in the start menu.

After this, you want to run the `vcvars32.bat` file from within your cmd shell. This file is specific to your MSVC installation. This will set environment varialbes that are used by cl.exe (the MSVC compiler) and link.exe (the MSVC linker) when compiling and linking the project.

I use *Visual Studio 2019 Community*, so this may look different for you. For my setup, this script is located here.

```
C:/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/VC/Auxiliary/Build/vcvars32.bat
```

If you are building MSVC 64bit, run `vcvars64.bat` instead. It should be within the same directory as `vcvars32.bat`. I would hate typing this path every time I get a tmux pane set up for building and I would be surprised if you didn't get annoyed too. To fix this, I created batch scripts that are in my path and I run them from the cmd shell depending on the types of builds I want to do.

```
< cl32.bat >
@echo off
C:/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/VC/Auxiliary/Build/vcvars32.bat
```

```
< cl64.bat >
@echo off
C:/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/VC/Auxiliary/Build/vcvars64.bat
```

Now it's just a matter of doing this...

```
cmd
cl[32|64].bat
```

...and everything is ready to go. The next step is to setup the Ninja generator by running CMake and finally build the project. From `build/msvc[32|64]/` run the following.

```
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel ../../
ninja
```

If you did everything properly and I didn't break something, everything should build.

### MinGW

*Disclaimer*: For me this is really easy, but depending on your development environment, it might be different for you. All I need to do is the following.

```
cmake -G "Ninja" ../../
ninja
```

The MinGW compiler is automatically selected and the binary is built. This works fine from my Cygwin shell, but if I do the same from the cmd shell, it does not work. I would look into this further to have a complete set of instructions here, but it is not important for me at this time.

## Build Issues?

### CMake is using a different compiler than the one you want it to.

You are going need a [CMake toolchain file](https://cmake.org/cmake/help/v3.6/manual/cmake-toolchains.7.html). It is important that you specify where the compiler is that you want to use with the following in your toolchain file.

```
set(CMAKE_C_COMPILER path/to/desired/compiler)
set(CMAKE_CXX_COMPILER path/to/desired/compiler)
```

Once you do that, you need to tell CMake about your toolchain file when invoking it.

```
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain/file ../../
```

More details are given in the [CMake toolchain documentation](https://cmake.org/cmake/help/v3.6/manual/cmake-toolchains.7.html).
