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

### Release

Recommended Compiler: `GCC`

Recommended Standard: `C11`

Dependencies:
- [`JSON-C`](https://github.com/json-c/json-c)

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
2. Add the following to System PATH:
```
C:\msys64\mingw64\include
C:\msys64\mingw64\bin
C:\msys64\mingw64\lib
C:\msys64\usr\bin
```

Run the following commands in MSYS2 terminal:
```
pacman -S base-devel
cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
```

Run the following in CMD from repository root directory
```
mkdir bin
cd bin
cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
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

### Derivative

Derivative is a new feature introduced in `v1.3`.

if derivative option is enabled (`--derivative`), the signals will be generated on `datasource`(considered here as `Index`) and position is taken on data from `derivative_datasource`.

`datasource` and `derivative_datasource` must have matching dates and are sorted.

 Derivative requires addition data source that should be specified in `config.json`:

```json
    "derivative": {
    
    "derivative_datasource": "NIFTY_JUL10500CE.csv",
    "derivative_interval": 0
    
    },
```

To use derivative mode, use `-d` arg.

---

## Feed Live data from a TCP socket.

(Linux Only)

from version `v1.4` live data can be fed into algoticks from a TCP socket. use `-s [PORT]` to listen to specific port on localhost(127.0.0.1) for data.

for example, client, refer to [stream_to_socket.py](tests/stream_to_socket.py)


### Callbacks

Callbacks are custom functions that are called if included in `config.json`

```json
    "callbacks": ["callbacks/log.so"],
```

---

## Testing

Algoticks uses [check.h Unit Testing Framework](https://libcheck.github.io/check/) for testing.

[How to install check.h?](https://libcheck.github.io/check/web/install.html)

To run all Tests:

```
cd tests
cmake .
make
./algoticks_tests
```

Output:
```
Running suite(s): Algoticks
100%: Checks: 13, Failures: 0, Errors: 0

```


---

### Timeline

Start of development: 13-Jun-2020
Repository made public: 02-Jul-2020

---

### Credits

Icons made by <a href="https://creativemarket.com/eucalyp" title="Eucalyp">Eucalyp</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>
