#include "ConfigTomlpp.hh"
#include <iostream>

using namespace std;

namespace cxfunc::config {

ConfigTomlpp::ConfigTomlpp(const string& filename) {
  theFilename = filename;
  theTree = toml::parse_file(filename);

  if (theTree["global"] && theTree["global"].is_table()) {
    theVerbose = theTree["global"]["verbose"].value_or(0);
  }
  if (theVerbose == 0 && theTree["verbose"]) {
    theVerbose = theTree["verbose"].value_or(0);
  }
  if (theVerbose > 0) {
    cout << theTree << '\n';
  }
}

}  // namespace cxfunc::config
