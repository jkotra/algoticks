<div align="center">

![](assets/algoticks_logo.png)


# Algoticks

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0) [![Build Status](https://travis-ci.org/jkotra/algoticks.svg?branch=master)](https://travis-ci.org/jkotra/algoticks)

---
<pre>
Algoticks is an algorithmic trading simulator written in C.
</pre>


[READ: Getting started with Algoticks](https://stdin.top/posts/getting-started-with-algoticks/)

[READ: Writing an Algorithm for Algoticks](https://stdin.top/posts/write-algo-for-algoticks/)

</div>

---

## Building

### Optimizations

Some optimizations are disabled by default to improve speed and efficiency.

```
#uncomment below line to enable Optimizations
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCHOMP -DQUOTED_CHECK -DCHECK_ROW_INTEGRITY")
```

### Release

Recommended Compiler: `GCC`

Recommended Standard: `C11`

Dependencies:
- [`JSON-C`](https://github.com/json-c/json-c)
- [`ZeroMQ`](https://github.com/zeromq/libzmq)
- [`libuv`](https://github.com/libuv/libuv)

```
mkdir bin
cd bin
cmake ..
make
```

upon successful compilation, the binary executable along with config files are found in `release` (`bin/release`) folder.

### Debug

```
mkdir bin
cd bin
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

upon successful compilation, the binary executable along with config files are found in `debug` (`bin/debug`) folder.

---


### Building on Windows (experimental)

1. Install [MSYS2](https://www.msys2.org/)
2. Install [CMake for Windows.](https://cmake.org/download/)
2. Add the following to System PATH:
```
C:\msys64\mingw64\include
C:\msys64\mingw64\bin
C:\msys64\mingw64\lib
C:\msys64\usr\bin
```

Run the following commands in `mingw64.exe` terminal:
```
pacman -S base-devel mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-json-c mingw-w64-x86_64-zeromq mingw-w64-x86_64-libuv
```

Run the following commands from repository root directory:
```
mkdir bin
cd bin
cmake -G "MSYS Makefiles" ..
make
```

---

## Usage

```
Usage: algoticks [OPTION...]
Algoticks - algorithmic trading simulator.

  -b, --benchmark            Benchmark mode
  -B, --benchmarkfile=FILE   Benchmark file.
  -C, --configfile=FILE      Config file.
  -d, --derivative           Derivative mode
  -D, --debug=LEVEL          Debug mode.
  -l, --live                 Live mode. Wait for date at EOF
  -s, --socket=PORT          Open socket to receive data at EOF
  -S, --settingsfile=FILE    settings file.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <jagadeesh@stdin.top>.


```


### Timeline

Start of development: 13-Jun-2020

Repository made public: 02-Jul-2020

---

### Credits

Icons made by <a href="https://creativemarket.com/eucalyp" title="Eucalyp">Eucalyp</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>
