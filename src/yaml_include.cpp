#include "yaml_include.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace {

class IncludeLoader {
 public:
  YAML::Node load(const std::filesystem::path& path) {
    std::error_code error;
    const auto resolved_path = std::filesystem::canonical(path, error);
    if (error) {
      throw std::runtime_error("Cannot resolve YAML file '" + path.string() +
                               "': " + error.message());
    }

    if (std::find(active_files_.begin(), active_files_.end(), resolved_path) !=
        active_files_.end()) {
      std::string chain;
      for (const auto& file : active_files_) {
        chain += file.string() + " -> ";
      }
      throw std::runtime_error("YAML include cycle: " + chain +
                               resolved_path.string());
    }

    active_files_.push_back(resolved_path);
    YAML::Node document;
    try {
      document = YAML::LoadFile(resolved_path.string());
    } catch (const YAML::Exception& e) {
      throw std::runtime_error("Cannot load YAML file '" +
                               resolved_path.string() + "': " + e.what());
    }
    std::vector<std::pair<YAML::Node, YAML::Node>> visited;
    YAML::Node result = resolve(document, resolved_path, visited);
    active_files_.pop_back();
    return result;
  }

 private:
  YAML::Node resolve(
      const YAML::Node& node, const std::filesystem::path& current_file,
      std::vector<std::pair<YAML::Node, YAML::Node>>& visited) {
    for (const auto& entry : visited) {
      if (node.is(entry.first)) {
        return entry.second;
      }
    }

    if (node.Tag() == "!include") {
      if (!node.IsScalar() || node.Scalar().empty()) {
        throw std::runtime_error("!include in '" + current_file.string() +
                                 "' must be a non-empty scalar path");
      }
      YAML::Node included = load(current_file.parent_path() / node.Scalar());
      visited.emplace_back(node, included);
      return included;
    }

    YAML::Node result(node.Type());
    if (node.IsScalar()) {
      result = node.Scalar();
    }
    result.SetTag(node.Tag());
    result.SetStyle(node.Style());
    visited.emplace_back(node, result);

    if (node.IsSequence()) {
      for (const auto& item : node) {
        result.push_back(resolve(item, current_file, visited));
      }
    } else if (node.IsMap()) {
      for (const auto& entry : node) {
        result[resolve(entry.first, current_file, visited)] =
            resolve(entry.second, current_file, visited);
      }
    }
    return result;
  }

  std::vector<std::filesystem::path> active_files_;
};

}  // namespace

YAML::Node load_yaml_with_includes(const std::filesystem::path& path) {
  return IncludeLoader().load(path);
}
