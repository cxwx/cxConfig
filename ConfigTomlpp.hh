#ifndef CONFIGTOMLPP_T_HH__
#define CONFIGTOMLPP_T_HH__

#ifdef HAVE_TOML

#include <string>
#include <toml++/toml.h>
#include "ConfigBase.hh"

namespace cxfunc::config {

class ConfigTomlpp : public ConfigBase {
 private:
  toml::table theTree;

 public:
  [[nodiscard]] auto Tree() const -> const toml::table& { return theTree; }

  auto operator[](const std::string& key) const -> toml::node_view<const toml::node> { return theTree[key]; }

  explicit ConfigTomlpp(const std::string& filename = std::string(std::getenv("HOME")) + "/rc/cxDefault.toml");
};

}  // namespace cxfunc::config
#endif  // CONFIGTOMLPP_T_HH__
#endif  // HAVE_TOML
