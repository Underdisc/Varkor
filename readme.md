# Varkor

Varkor is a C++ game engine. It can run as a standalone executable, but its true intent is to be used a static library that other C++ projects can link against and build upon. Fundamentally, it's a game engine, but, like other engines, its use cases extend beyond just games.

## Building Varkor Standalone

*Disclaimer*: This is how I build the project. Feel free to use your own method if you are inclined. No promises that you won't encounter some bumps though.

*Before You Build*: Make sure you initialize the submodules that Varkor uses. From anywhere in the repo, run the following to pull them.

```
git submodule init
git submodule update
```

*Once You Build*: After you build the project, you'll find `varkor.exe` in `{repo_root}/working/`. This directory contains all the files that varkor may access at runtime. This is why the Varkor binary is placed here.

At the moment, Varkor only builds under Windows, but will be expanded in the near future for building under Linux as well. On Windows, Varkor can be built under MSVC. To build Varkor, you are going to need a build of CMake and Ninja. I build everything on command line. If you don't already have it, I highly recommend Cygwin as well.

- CMake:  https://cmake.org/download/
- Ninja:  https://github.com/ninja-build/ninja/releases
- Cygwin: https://www.cygwin.com/

Once you have those, make sure *ninja.exe* and *cmake.exe* are in your path.

### MSVC

Create a build directory at the root of your local copy of the repo. This directory is ignored by git.

```
mkdir build
cd build
```

Within the build directory, I like to have multiple directories for different build types. Right now I have `rel/msvc64/` and `dbg/msvc64/` in my build directory. How you organize this is up to you.

```
mkdir rel
cd rel
mkdir msvc64
cd msvc64
```

The next step is entering the cmd shell that comes with Windows. There are two ways to do this. You can choose to simply open the command prompt, the typical way of accessing the shell. You can also just run `cmd` from your own terminal to access the shell. I use the [Mintty](https://mintty.github.io/) terminal that comes with Cygwin and the latter is how I access the shell.

After this, you want to run `vcvars64.bat` in the shell. This file is specific to your MSVC installation. This will set environment varialbes that are used by `cl.exe` (the MSVC compiler) and `link.exe` (the MSVC linker) when compiling and linking the project.

I use *Visual Studio 2019 Community*, so this may look different for you. For my setup, this script is located here.

```
C:/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/VC/Auxiliary/Build/vcvars64.bat
```

I would hate typing this path every time I get a terminal set up for building and I would be surprised if you didn't get annoyed too. To fix this, I created batch scripts that are in my path and I run them from the cmd shell depending on the types of builds I want to do.

```
< cl64.bat >
@echo off
C:/"Program Files (x86)"/"Microsoft Visual Studio"/2019/Community/VC/Auxiliary/Build/vcvars64.bat
```

Now getting a terminal ready for building only involves running the commands below.

```
cmd
cl64.bat
```

The next step is to setup the Ninja generator by running CMake. From `build/rel/msvc64/` run the following.

```
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel ../../../
```

If you are looking to make debug builds, do this instead.

```
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../../../
```

Now you should have a `build.ninja` file in your current directory. All that's left to do is build. If you did everything properly and I didn't break something, the varkor binary can be built with the following.

```
ninja varkorStandalone
```

### Rapid Builds

Because `varkor.exe` is placed in `working/` by default, building Varkor, going to `working/` to run it, and going back to build again can be really fucking annoying. Inside of `working/`, you will find a batch script called `build.bat`. After setting up the cmd shell and creating a `buildSpecs.bat` file, you can run `build.bat`. The script will go to a specified build directory, run the generator command, and return to `working/`. `build r` will also run varkor if the build was successful.

As mentioned, you need to create a file called `buildSpecs.bat` to use `working/build.bat`. This file is git ignored and must be placed in `working/`. The script needs to set environment variables specific to your setup so they can be used by `build.bat`. You can find out what these environment variables are at the top of `working/checkBuildSpecs.bat`. As an example, here is the current content of my `buildSpecs.bat` file.

```
set buildDir="../build/rel/msvc64"
set generator=ninja
set target=varkor
```
