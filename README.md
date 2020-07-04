<div align="center">

![](assets/algoticks_logo.png)


# Algoticks

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

---
<pre>
Algoticks is an algorithmic trading simulator written in C.
</pre>



[READ: Getting started with Algoticks](https://stdin.top/posts/getting-started-with-algoticks/)

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

### Timeline

Start of development: 13-Jun-2020

Repository made public: 02-Jul-2020

---

### Credits

Icons made by <a href="https://creativemarket.com/eucalyp" title="Eucalyp">Eucalyp</a> from <a href="https://www.flaticon.com/" title="Flaticon"> www.flaticon.com</a>