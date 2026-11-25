#pragma once

#include <filesystem>
#include <yaml-cpp/yaml.h>

YAML::Node load_yaml_with_includes(const std::filesystem::path& path);
