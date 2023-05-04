# `computational-graph`

[![Linux build status](https://github.com/acd1034/computational-graph/actions/workflows/linux-build.yml/badge.svg)](https://github.com/acd1034/computational-graph/actions/workflows/linux-build.yml)
[![macOS build status](https://github.com/acd1034/computational-graph/actions/workflows/macos-build.yml/badge.svg)](https://github.com/acd1034/computational-graph/actions/workflows/macos-build.yml)

## Contents

```
.
├── README.md
├── CMakeLists.txt
├── include
│   └── iris
│       ├── graph.hpp
│       ├── eliminate_nop.hpp
│       ├── insert_nop_after_opa.hpp
│       ├── memory_usage_analyzer.hpp
│       └── ...
├── tests
│   ├── main
│   │   └── main.cpp
│   └── ...
└── ...
```

The correspondence between the files and the questions is as follows:

- [include/iris/graph.hpp](include/iris/graph.hpp):
  Q1. (Implement Node, Value, and Graph)
- [include/iris/eliminate_nop.hpp](include/iris/eliminate_nop.hpp):
  Q2. (Remove Node "NOP")
- [include/iris/insert_nop_after_opa.hpp](include/iris/insert_nop_after_opa.hpp):
  Q3. (Add Node "NOP" after the output of Node "opA")
- [include/iris/memory_usage_analyzer.hpp](include/iris/memory_usage_analyzer.hpp):
  Q4. (Estimate memory consumption)
- [tests/main/main.cpp](tests/main/main.cpp):
  Tests for these implementations

## How to Run Tests

```sh
$ cmake -S . -B build
$ cmake --build build
$ ctest --test-dir build
```

Catch2 testing framework is used for testing. Catch2 is automatically fetched by CMake.
