# Varkor

Varkor is a C++ game engine. It can run as a standalone executable, but its true intent is to be used a static library that other C++ projects can link against and build upon. Fundamentally, it's a game engine, but, like other engines, its use cases extend beyond just games.

## Building Varkor Standalone

*Disclaimer*: This is how I build the project. Feel free to use your own method if you are inclined. No promises that you won't encounter some bumps though.

*Before You Build*: Make sure you initialize the submodules that Varkor uses. From anywhere in the repo, run the following to pull them.

```
git submodule init
git submodule update
```

Varkor builds on Windows under MSVC and Clang 64bit and on Linux under Clang 64bit. To build you will need CMake and Ninja. If you don't already have it, I highly recommend Cygwin as well.

- CMake:  https://cmake.org/download/
- Ninja:  https://github.com/ninja-build/ninja/releases
- Cygwin: https://www.cygwin.com/

Once you have those, make sure *ninja.exe* and *cmake.exe* are in your path.

#### Linux

Install all of the needed packages.

```
sudo apt install clang
sudo apt install cmake
sudo apt install ninja
```

Notes for better stack traces. You need to install libdw.

```
sudo apt-get install libdw-dev
```



### General

Go to `working/` within your local copy of the repo and run `initializeBuildDirs.bat` 

```
cd path/to/repo/
cd working/
initializeBuildDirs
```

This will create all of the build directories for compilers and configurations that are supported by Varkor. When you look back at the root directory you will find the following.

```
build/
| clang64/
| | dbg/
| | rel/
| | relDbg/
| | relMin/
| msvc64/
| | dbg/
| | rel/
| | relDbg/
| | relMin/
```

From here, things differ slightly depending on MSVC or Clang. Both examples create debug builds.

##### MSVC

Enter the cmd shell that comes with Windows. There are two ways to do this. You can simply open the command prompt or you can run `cmd` from your preferred terminal to access the shell. I use the [Mintty](https://mintty.github.io/) terminal that comes with Cygwin and the latter is how I access the shell.

Next, you need to run `vcvars64.bat`. This file is specific to your MSVC installation. This will set environment varialbes that are used by `cl.exe` (the MSVC compiler) and `link.exe` (the MSVC linker) when compiling and linking the project. For my installation of *Visual Studio 2019 Community* this script is located here.

```
C:/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/VC/Auxiliary/Build/vcvars64.bat
```

Typing this every time to set up a shell for MSVC building is annoying. To fix this, create a batch script that is in your path and run it with the cmd shell. Mine is called `cl64.bat`

```
< cl64.bat >
@echo off
C:/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/VC/Auxiliary/Build/vcvars64.bat
```

Now getting a terminal ready for MSVC building only involves running the commands below.

```
cmd
cl64
```

Go to the directory for the desired build type, run CMake, and build the `varkorStandalone` target.

```
cd root/build/msvc64/dbg
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../../../
ninja varkorStandalone
```

##### Clang

Go to the directory for the desired build type, run CMake, and build the `varkorStandalone` target.

```
cd root/build/clang64/dbg
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=llvm/installation/path/bin/clang.exe -DCMAKE_CXX_COMPILER=llvm/installation/path/bin/clang++.exe ../../../
ninja varkorStandalone
```

#### Build Type Argument

The `CMAKE_BUILD_TYPE` argument differs depending on configuration.

```
dbg - Debug
rel - Release
relDbg - RelWithDebInfo
relMin - MinSizeRel
```

### Running Varkor Standalone

All built executables must run from `working/`. This directory contains `vres/`, which stands for Varkor resources. It contains files Varkor expects to exist during initialization. If they aren't there, Varkor will crash during initialization. Move the binary you built into `working/` and run it from there.

```
cd path/to/build/configuration
move varkorStandalone.* ../../../working
cd ../../../working
varkorStandalone.exe
```

### Rapid Runs

Because binaries must run from `working/`, building a binary, moving it to `working/`, moving the prompt to `working/`, running it, and potentially moving it back again is extremely fucking annoying. Inside of `working/`, you will find a batch script called `build.bat`. The script will handle the entire building and running process. It allows you to specify the compiler, configuration, target, and whether to run it or not. Look at `working/build.bat` for more details.