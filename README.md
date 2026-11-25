# yaml-cpp `!include` example

This C++17 example loads YAML with [yaml-cpp](https://github.com/jbeder/yaml-cpp)
and replaces `!include` tags with the contents of another YAML file. yaml-cpp
does not implement `!include` itself; the loader in this project adds it.

## Build and run

Install CMake, a C++17 compiler and yaml-cpp (for example, on macOS:
`brew install cmake yaml-cpp`). Then run:

```sh
cmake -S . -B build
cmake --build build
./build/yaml_include_example examples/config.yaml
ctest --test-dir build --output-on-failure
```

The program prints the resolved YAML.

For a root-level include, run:

```sh
./build/yaml_include_example examples/root-include.yaml
```

The [root-level example](examples/root-include.yaml) includes the whole
`config.yaml` document, including its nested includes.

To use the result in C++, call
`load_yaml_with_includes(path)` from `include/yaml_include.hpp` and read the
returned `YAML::Node` normally:

```cpp
#include "yaml_include.hpp"

YAML::Node config = load_yaml_with_includes("examples/config.yaml");
int port = config["database"]["port"].as<int>();
```

Loading and conversion errors throw exceptions, so handle them in your
application as shown in `src/main.cpp`.

## Include syntax

```yaml
database: !include settings/database.yaml
```

The tag must have a non-empty scalar path. Paths are relative to the file
containing the tag, not the current working directory; absolute paths also
work. An included document can itself contain includes. Includes can replace
the root, a mapping value, or a sequence item. Missing files, invalid tags,
parse errors and include cycles are reported as errors.

The loader reads paths specified by the YAML without restricting them to a
particular directory. Only use it with trusted YAML and trusted included files.