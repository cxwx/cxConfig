#include "ConfigYAML.hh"
#include <iostream>

using namespace std;

namespace cxfunc::config {

ConfigYAML::ConfigYAML(const string& filename) {
  theFilename = filename;
  theConfig = YAML::LoadFile(filename);

  if (theConfig["global"]) {
    if (theConfig["global"]["verbose"].IsScalar())
      theVerbose = theConfig["global"]["verbose"].as<int>();
  }

  if (theVerbose > 0)
    YAML::Dump(theConfig);
}

}  // namespace cxfunc::config
