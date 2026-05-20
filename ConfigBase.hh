#ifndef CONFIGBASE_HH__
#define CONFIGBASE_HH__

#include <string>

namespace cxfunc::config {

class ConfigBase {
 protected:
  int theVerbose = 0;
  std::string theFilename;

 public:
  [[nodiscard]] int Verbose() const { return theVerbose; }
  [[nodiscard]] const std::string& Filename() const { return theFilename; }
  virtual ~ConfigBase() = default;

  ConfigBase(const ConfigBase&) = delete;
  ConfigBase& operator=(const ConfigBase&) = delete;
  ConfigBase(ConfigBase&&) = delete;
  ConfigBase& operator=(ConfigBase&&) = delete;

 protected:
  ConfigBase() = default;
};

}  // namespace cxfunc::config

#endif  // CONFIGBASE_HH__
