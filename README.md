# LC-3 (Little Computer 3) Virtual Machine

## Prerequisites

- GCC
- CMake > 3.1.0

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
