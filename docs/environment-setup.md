# Environment Setup
## Ubuntu

welab is first developed on Ubuntu 20.04 LTS, this is recommended on this stage.

## Python Environment

welab is a software that combine c++, python3 and electron together. So you need to install a conda environment
to make everything work. python3.8 is used during develop.

## conan2

welab use conan2 to manage c++ modules, so you need to install conan2 in you python3 conda environment.
```
python3 -m pip install conan2
```

## build

```
cd welab/modules
conan install . --build=gtest/1.14.0
conan build . -c tools.build:skip_test=False
```

## package

```
conan create . --build=missing -tf=""
```