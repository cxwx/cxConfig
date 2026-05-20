#ifndef CONFIGMANAGER_HH__
#define CONFIGMANAGER_HH__

#include <string>
#include <unordered_map>
#include <stdexcept>
#include "ConfigBase.hh"

namespace cxfunc::config {

class ConfigManager {
 private:
  std::unordered_map<std::string, ConfigBase*> theConfigs;

 public:
  void Register(const std::string& name, ConfigBase* cfg) {
    theConfigs[name] = cfg;
  }

  [[nodiscard]] ConfigBase* Get(const std::string& name) const {
    auto it = theConfigs.find(name);
    if (it == theConfigs.end()) {
      return nullptr;
    }
    return it->second;
  }

  template <typename T>
  [[nodiscard]] T* GetAs(const std::string& name) const {
    return dynamic_cast<T*>(Get(name));
  }

  [[nodiscard]] size_t Size() const { return theConfigs.size(); }
  [[nodiscard]] bool Has(const std::string& name) const {
    return theConfigs.count(name) > 0;
  }
};

}  // namespace cxfunc::config

#endif  // CONFIGMANAGER_HH__
