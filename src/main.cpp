#include "yaml_include.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: yaml_include_example <file.yaml>\n";
    return 1;
  }

  try {
    const YAML::Node config = load_yaml_with_includes(argv[1]);
    YAML::Emitter emitter;
    emitter << config;
    if (!emitter.good()) {
      throw std::runtime_error("Cannot emit YAML: " + emitter.GetLastError());
    }

    std::cout << emitter.c_str() << '\n';
    std::cout.flush();
    if (!std::cout) {
      throw std::runtime_error("Cannot write YAML to standard output");
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}
