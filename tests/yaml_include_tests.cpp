#include "yaml_include.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void require(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void expect_error(const std::filesystem::path& path,
                  const std::string& expected) {
  try {
    load_yaml_with_includes(path);
  } catch (const std::exception& e) {
    require(std::string(e.what()).find(expected) != std::string::npos,
            "Unexpected error: " + std::string(e.what()));
    return;
  }
  throw std::runtime_error("Expected an error for " + path.string());
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Expected example and fixture directories\n";
    return 1;
  }

  try {
    const std::filesystem::path examples = argv[1];
    const std::filesystem::path fixtures = argv[2];
    const YAML::Node config = load_yaml_with_includes(examples / "config.yaml");
    require(config["application"].as<std::string>() == "demo",
            "Ordinary values must remain intact");
    require(config["database"]["port"].as<int>() == 5432,
            "Mapping include failed");
    require(config["database"]["credentials"]["username"].as<std::string>() ==
                "example",
            "Nested relative include failed");
    require(config["services"][0]["name"].as<std::string>() == "api",
            "Sequence include failed");
    require(config["services"][1]["name"].as<std::string>() == "local-worker",
            "Ordinary sequence item changed");
    require(load_yaml_with_includes(fixtures / "root.yaml")["name"]
                .as<std::string>() == "api",
            "Root include failed");
    const YAML::Node repeated = load_yaml_with_includes(fixtures / "repeated.yaml");
    require(repeated["first"]["name"].as<std::string>() == "api" &&
                repeated["second"]["name"].as<std::string>() == "api",
            "Repeated includes must not be treated as cycles");

    const YAML::Node aliases = load_yaml_with_includes(fixtures / "aliases.yaml");
    require(aliases["defaults"].is(aliases["copy"]),
            "Aliases must retain their shared identity");
    require(aliases["copy"]["service"]["name"].as<std::string>() == "api",
            "Include within an alias failed");
    require(aliases["recursive"].is(aliases["recursive"]["self"]),
            "Recursive aliases must retain their shared identity");

    expect_error(fixtures / "cycle-a.yaml", "YAML include cycle");
    expect_error(fixtures / "invalid.yaml", "must be a non-empty scalar path");
    expect_error(fixtures / "empty.yaml", "must be a non-empty scalar path");
    expect_error(fixtures / "missing.yaml", "does-not-exist.yaml");
    expect_error(fixtures / "parse-error.yaml", "parse-error.yaml");
  } catch (const std::exception& e) {
    std::cerr << "Test failed: " << e.what() << '\n';
    return 1;
  }
  std::cout << "All include tests passed\n";
}
