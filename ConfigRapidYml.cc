#include "ConfigRapidYml.hh"
#include <exception>
#include <fstream>
#include <iostream>

using namespace std;

namespace cxfunc::config {

ConfigRapidYml::ConfigRapidYml(const string& filename) {
  theFilename = filename;
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filename);
  }
  size_t size = file.tellg();
  buffer.resize(size + 1, '\0');
  file.seekg(0);
  file.read(buffer.data(), size);
  theTree = ryml::parse_in_place(ryml::to_substr(buffer.data()));
  theConfig = theTree.rootref();

  if (!theConfig.has_child("global")) {
    cerr << "Config file should has a global child!\n";
    std::terminate();
  }
  ryml::ConstNodeRef global = theConfig["global"];

  if (global.has_child("verbose")) {
    global["verbose"] >> theVerbose;
  }

  if (theVerbose > 0) {
    ryml::emit_yaml(theTree);
  }
}

}  // namespace cxfunc::config
