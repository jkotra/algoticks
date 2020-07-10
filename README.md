<div align="center">

![](assets/algoticks_logo.png)


# Algoticks

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

---
<pre>
Algoticks is an algorithmic trading simulator written in C.
</pre>



[READ: Getting started with Algoticks](https://stdin.top/posts/getting-started-with-algoticks/)

[READ: Writing an Algorithm for Algoticks](https://stdin.top/posts/write-algo-for-algoticks/)

</div>

---

## Building (Linux)

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

### Building on Windows

**Note**:  *Algoticks* is slower in windows compared to Linux version. Windows optimization is being worked on, but is not a priority.

* Install [JSON-C](https://github.com/json-c/json-c#building-on-unix-and-windows-with-vcpkg-)
* [Install C and C++ support in Visual Studio](https://docs.microsoft.com/en-us/cpp/build/vscpp-step-0-installation?view=vs-2019)
* Install [Clang](https://releases.llvm.org/download.html)
* [Ninja Build](https://github.com/ninja-build/ninja/releases)

`include` path is set to `C:/vcpkg/installed/x64-windows/include` in `CMakeLists.txt`. Edit the path if you installed `vcpkg` in some other directory.

`lib` path is set to `C:/vcpkg/installed/x64-windows/lib` in `CMakeLists.txt`. Edit the path if you installed `vcpkg` in some other directory.

```
mkdir bin
cd bin

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release ..
ninja
```

---

## Usage

```
-V			Print Version and Exit.
-H			Print this message and Exit.
-D			Enable Debug.
-L			Indicate datasource is updated in realtime.


--settings [*.JSON]			Custom settings file. Default: settings.json
--config [*.JSON]			Custom config file. Default: config.json
--benchmark -B [(Optional)[*.JSON]	Custom benchmark file. Default: benchmark.json


```
---

## Tests

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
