# LC-3 (Little Computer 3) Virtual Machine

The Lc3 is an educational CPU, that is not physically implemented, so as a personal educational project I made a virtual machine that can execute code targeted to the Lc3.

## Prerequisites
There are no many prerequisites, only need a C compiler, CMake, and maybe make or visual studio, i test it with make.

- GCC (Tested in gcc-4.6.3)
- Make (Tested in GNU Make 4.2.1)
- CMake >= 3.1.0

## Building project

Fist need to create a folder to save generated build files.

```bash
mkdir build && cd build
```

Inside the build folder call cmake to the project.

```bash
cmake ../
```

Now you can build the project using cmake or any you want.

```bash
cmake --build .
```

## Usage

Call the executable and as argument the path to the object file of your code.

```bash
lc3vm [obj-file]
```

A simulation log file will generate called "lc3vm.log", used to debug the application.

## Based on

I use the information provided in the next repository: [LC3-VM](https://github.com/justinmeiners/lc3-vm)
