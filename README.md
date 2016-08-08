# fast-mcmc
[![Build Status](https://travis-ci.org/mkleesiek/fast-mcmc.svg?branch=master)](https://travis-ci.org/mkleesiek/fast-mcmc)

A lightweight and extensible Markov Chain Monte Carlo (MCMC) library written in modern C++.

## Introduction
With this project I aim at building a modular **C++ framework** for performing **Bayesian inference** and **mathematical optimization** tasks in **high-dimensional models**.

While there are several MCMC algorithms available for the software environment [R](https://www.r-project.org/), similar options for the C++ language, which is still the more popular choice for scientific high-performance computations, are limited.

In order to allow for a future proof and robust code base with intuitive extensibility and good integrability of the library into existing C++ projects, I'd like to meet the following design criteria:
- Limited use of 3rd party libraries for better **maintainability** and control of dependencies.
  - The only mandatory requirement for now are the [Boost C++ libraries](http://www.boost.org/). Most of them are peer-reviewed, well documented and follow decent style guidelines. Especially the numeric, linear algebra and statistical utilities are extremely helpful in the start of this project.
  - Although popular among scientists, I have ruled out to interface with [ROOT](https://root.cern.ch) directly. Its API is cluttered, the code style heavily disputed and linux installation packages are not reliably maintained.
- Modern **object-oriented C++11** style with dashes of functional programming and templating magic where justifiable.
- Thread-safety to make way for multi-core parallelization.
- Unit testing (Google Test) and code annotations (Doxygen).

## Status
- Build system, unit testing and continuous integration set up.
- Numeric + logging utilities and random number generator interfaces implemented.
- Basic classes and interfaces declared.

## Supported compilers and operating systems
Essentially, all C++11 compliant compilers should work.
Currently the build is being tested on [Travis CI](https://travis-ci.org/mkleesiek/fast-mcmc.svg?branch=master) using the following OS/compiler combinations:
- Ubuntu Trusty (14.04)
  - g++ 4.8.4
  - g++ 5.3.0
  - g++ 6.1.1
  - clang++ 3.5
  - clang++ 3.9
- OS X El Capitan (10.11.5)
  - Xcode (clang) 7.3

## Prerequisits
- [Meson build system](http://mesonbuild.com/) with [Ninja](https://ninja-build.org/) as backend
- [Boost C++ libraries](http://www.boost.org/) (>= 1.45.0)
- Optional:
  - [Doxygen](http://www.doxygen.org)
  - Intel's [Threading Building Blocks](https://www.threadingbuildingblocks.org/)
  - Google's C++ test framework [Google Test](https://github.com/google/googletest)

On Debian-based systems simply use apt-get:
```
sudo apt-get install meson ninja libboost-all-dev doxygen libtbb-dev libgtest-dev 
```
If meson is not provided by your package manager, you can use the python 3 installer:
```
pip3 install meson
```

## Quick Installation
- Setup meson in an out-of-source build directory (e.g. a subdirectory in the source root):
```
cd fast-mcmc
mkdir build
cd build
meson .. [--prefix=/install/dir] [--buildtype=(plain|debug|debugoptimized|release)]
```
- Use mesonconf to revise or modify your configuration:
```
mesonconf
```
- Both meson(.py) and mesonconf(.py) offer decent command-line help if executed with the option --help.
- To compile the code, execute ninja in the build directory:
```
ninja [-j NTHREADS] [-v]
```
- Install:
```
ninja install
```
- Run unit tests:
```
ninja test
```
- Generate source documentation (requires doxygen):
```
ninja doc
```
