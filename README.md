# Night Adder

Night Adder is a multithreaded, lock-free physics engine that makes use of
Velan Studio's [Viper Engine](https://github.com/VelanStudios/viper-engine).
It provides support for Windows, Linux, and Mac, but **it has currently only
been tested on Linux**.

Linux (and Mac) support for the Viper Engine is available through the
[linux-support
branch](https://github.com/wphicks/viper-engine/tree/linux-support) of my fork
of Viper. Tests are written with [Google Test
(gtest)](https://github.com/google/googletest). Assuming you have gtest
available on your system, you can build and run the Night Adder tests on Linux
by doing the following:

```bash
mkdir GameEngine
cd GameEngine
git clone https://github.com/wphicks/viper-engine.git
git clone https://github.com/wphicks/night-adder.git
cd viper-engine
git checkout -b develop origin/develop
cd ../night-adder
make init
make test
```

By default, Night Adder will be built with clang, but setting command line
option `GCC` to 1 will switch the compiler to gcc. Other command line settings
include `OPT` to set the optimization level, `RELEASE` to suppress debug
symbols, and `PROFILE` to enable profiling.
