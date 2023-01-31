# Varkor
Varkor is a C++ game engine. It can run as a standalone executable, but its true intent is to be used a static library that other C++ projects can link against and build upon. Fundamentally, it's a game engine, but, like other engines, its use cases extend beyond just games.

## Building Varkor Standalone
*Disclaimer*: This is how I build the project. Feel free to use your own method if you are inclined. No promises that you won't encounter some bumps though.
*Before You Build*: Make sure you initialize the submodules that Varkor uses. From anywhere in the repo, run the following to pull them.
```
git submodule init
git submodule update
```
Varkor builds on Windows under MSVC and Clang 64bit and on Linux under Clang and G++ 64bit.

### General
Create a build directory in the root of your repo. In this build directory, create another directory for the compiler you'll be using. Within that directory, create another directory for the configuration type you are building.
```
cd repoRoot
mkdir build && cd build
mkdir compiler && cd compiler
mkdir configuration && cd configuration
```
It's not absolutely necessary to use this structure, but I highly recommend it because there are scripts that let you build and run the project with a single command and they expect the build directory to be organized like this. As an example, here is what my build directory looks like.
```
build/
|-clang64/
| |-dbg/
| |-rel/
| |-relDbg/
| |-relMin/
|-gpp64/
  |-dbg/
  |-rel/
  |-relDbg/
  |-relMin/
```
The configuration directories map directly to the different values of CMAKE_BUILD_TYPE.
```
dbg ----> Debug
rel ----> Release
relDbg -> RelWithDebInfo
relMin -> MinSizeRel
```

## Linux
Install the necessary packages.
```
sudo apt install cmake
sudo apt install ninja
```

### g++
Go to the build directory for the desired configuration, run CMake, and build `varkorStandalone`. Here is an example for a g++ debug build.
```
cd build/gpp64/dbg
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/path/to/gcc -DCMAKE_CXX_COMPILER=/path/to/g++ ../../../
ninja varkorStandalone
```

### clang
Install clang.
```
sudo apt install clang
```
Go to the build directory for the desired configuration, run CMake, and build `varkorStandalone`. Here is an example for a clang release build.
```
cd build/clang64/dbg
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/path/to/clang -DCMAKE_CXX_COMPILER=/path/to/clang++ ../../../
ninja varkorStandalone
```

### Details
Anything linked against the Varkor library will print out a stack trace when a crash occurs. Install dwarf for better stack traces.
```
sudo apt-get install libdw-dev
```

## Windows
To build you will need CMake and Ninja. If you don't already have it, I highly recommend Cygwin as well.
- CMake:  https://cmake.org/download/
- Ninja:  https://github.com/ninja-build/ninja/releases
- Cygwin: https://www.cygwin.com/
Once you have those, make sure *ninja.exe* and *cmake.exe* are in your path.

### MSVC
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
Go to the directory for the desired configuration, run CMake, and build `varkorStandalone`.
```
cd root/build/msvc64/dbg
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../../../
ninja varkorStandalone
```

### Clang
Go to the directory for the desired compiler and configuration, run CMake, and build `varkorStandalone`.
```
cd root/build/clang64/dbg
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=path/to/clang.exe -DCMAKE_CXX_COMPILER=path/to/clang++.exe ../../../
ninja varkorStandalone
```

## Running Varkor Standalone
All binaries must run from `working/`. This directory contains `vres/`, which stands for *Varkor resources*. It contains files Varkor expects to exist during initialization. If they aren't there, Varkor will crash during initialization. Move the binary you built into `working/` and run it from there.
```
cd path/to/build/configuration
move varkorStandalone.* ../../../working
cd ../../../working
varkorStandalone.exe
```

### Rapid Runs
Because binaries must run from `working/`, building a binary, moving it to `working/`, moving the prompt to `working/`, running it, and potentially moving it back again is extremely fucking annoying. Inside of `script/`, you will find `build.js`. This is a nodejs script that streamlines running varkor binaries. Look inside the script for more details. You will need nodejs in order to run it. For windows users, https://nodejs.org/en/ and make sure *node* is in your path.
```
sudo apt install node
```
Here's an example invocation of the script.
```
node build clang64 dbg varkorStandalone r
```