# `computational-graph`

[![Linux build status](https://github.com/acd1034/computational-graph/actions/workflows/linux-build.yml/badge.svg)](https://github.com/acd1034/computational-graph/actions/workflows/linux-build.yml)
[![macOS build status](https://github.com/acd1034/computational-graph/actions/workflows/macos-build.yml/badge.svg)](https://github.com/acd1034/computational-graph/actions/workflows/macos-build.yml)

## How to run tests

```sh
$ cmake -S . -B build
$ cmake --build build
$ ctest --test-dir build
```

## File organization

- 問 1 (Node, Value, Graph の実装): [include/iris/graph.hpp](include/iris/graph.hpp)
- 問 2 ("NOP" の削除): [include/iris/eliminate_nop.hpp](include/iris/eliminate_nop.hpp)
- 問 3 ("opA" の出力に "NOP" を追加): [include/iris/insert_nop_after_opa.hpp](include/iris/insert_nop_after_opa.hpp)
- 問 4 (メモリ消費量の予測): [include/iris/memory_usage_analyzer.hpp](include/iris/memory_usage_analyzer.hpp)
- これらの実装のテストが [tests/main/main.cpp](tests/main/main.cpp) にあります
