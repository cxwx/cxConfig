#ifndef CONFIGJSON_T_HH__
#define CONFIGJSON_T_HH__

#include <nlohmann/json.hpp>
#include <string>
#include "ConfigBase.hh"

namespace cxfunc::config {

class ConfigJson : public ConfigBase {
 private:
  nlohmann::json theConfig;

 public:
  explicit ConfigJson(const std::string& filename = std::string(std::getenv("HOME")) + "/rc/cxDefault.json");

  [[nodiscard]] const nlohmann::json& Config() const { return theConfig; }

  nlohmann::json& operator[](const std::string& key) { return theConfig[key]; }
  const nlohmann::json& operator[](const std::string& key) const { return theConfig[key]; }

  template <typename T>
  const nlohmann::json& operator()(const nlohmann::json& node, const T& key) const {
    return node[key];
  }

  template <typename T, typename... Args>
  const nlohmann::json& operator()(const nlohmann::json& node, const T& key, Args... args) const {
    return operator()(node[key], args...);
  }

  template <typename... Args>
  const nlohmann::json& operator()(Args... args) const {
    return operator()(theConfig, args...);
  }
};

}  // namespace cxfunc::config

#endif  // CONFIGJSON_T_HH__
