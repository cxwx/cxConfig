#include "ConfigJson.hh"
#include <fstream>
#include <iostream>

using namespace std;

namespace cxfunc::config {

ConfigJson::ConfigJson(const string& filename) {
  theFilename = filename;
  ifstream file(filename);
  if (!file) {
    throw runtime_error("Failed to open file: " + filename);
  }
  file >> theConfig;

  if (theConfig["global"]["verbose"].is_number_integer()) {
    theVerbose = theConfig["global"]["verbose"].get<int>();
  } else if (theConfig["verbose"].is_number_integer()) {
    theVerbose = theConfig["verbose"].get<int>();
  }
}

}  // namespace cxfunc::config
