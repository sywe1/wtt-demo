<img src="https://github.com/sywe1/wtt-demo/blob/master/screenshots/screenshots.gif" width="850" alt="screenshots" />

Wavelet Tranform Toolkit (WTT) Demo
==================

Dependency
------------

* g++ (7.2+) or clang++ (5+)
* CGAL (4.2+)
* Boost (1.58+)
* Qt (5.9+)
* [WTT](https://github.com/uvic-aurora/wtt)

Build
----------------

This program is designed for demonstrating the usage of the WTT. In order to build this program, the WTT should be installed on the machine. In what follows, let `$WTT_DIR` denote the directory where the WTT is installed; let `$TOP_DIR` denote the top-level directory; let `$BUILD_DIR` denote a new directory to be created for building the program.

To build with CMake, run the command sequence:

```shell
cmake -H$TOP_DIR -B$BUILD_DIR -DCMAKE_PREFIX_PATH=$WTT_DIR
cmake --build $BUILD_DIR
```

To execute the program, run the command:

```shell
$BUILD_DIR/demo
```
