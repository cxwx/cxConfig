#include <iostream>
#include <string>

#include "ConfigYAML.hh"
#include "ConfigJson.hh"
#include "ConfigTomlpp.hh"
#include "ConfigRapidYml.hh"
#include "ConfigManager.hh"

using namespace std;
using namespace cxfunc::config;

// ==================== helper ====================

static void check(bool cond, const string& msg) {
  if (!cond) {
    cerr << "FAIL: " << msg << endl;
    exit(1);
  }
  cout << "  PASS: " << msg << endl;
}

// ==================== ConfigYAML tests ====================

static void test_yaml_from_file() {
  string path = string(getenv("HOME")) + "/software/config/git/test/config.yaml";

  ConfigYAML cfg(path);

  check(cfg.Verbose() == 2, "yaml Verbose == 2");
  check(cfg.Config()["app"]["name"].as<string>() == "test_app", "yaml app.name");
  check(cfg.Config()["app"]["version"].as<int>() == 3, "yaml app.version");
  check(cfg.Config()["database"]["host"].as<string>() == "localhost", "yaml db host");
  check(cfg.Config()["database"]["pool"]["size"].as<int>() == 10, "yaml db pool size");

  // operator[] access
  check(cfg["app"]["name"].as<string>() == "test_app", "yaml [] app.name");

  // operator() chaining
  auto val = cfg("app", "name");
  check(val.as<string>() == "test_app", "yaml () chain access");

  check(cfg["database"]["pool"]["timeout"].as<int>() == 30, "yaml nested via []");
  check(cfg("database", "pool", "timeout").as<int>() == 30, "yaml nested via ()");

  // base class
  check(cfg.Filename() == path, "yaml Filename()");

  cout << "  ConfigYAML all tests passed" << endl;
}

// ==================== ConfigJson tests ====================

static void test_json_basic() {
  string path = string(getenv("HOME")) + "/software/config/git/test/config.json";

  ConfigJson cfg(path);

  check(cfg.Verbose() == 3, "json Verbose == 3");
  check(cfg.Config()["app"]["name"].get<string>() == "json_test", "json app.name");
  check(cfg.Config()["app"]["version"].get<int>() == 4, "json app.version");
  check(cfg.Config()["database"]["host"].get<string>() == "remotehost", "json db host");
  check(cfg.Config()["database"]["pool"]["size"].get<int>() == 20, "json db pool size");

  // operator[] access
  check(cfg["app"]["name"].get<string>() == "json_test", "json [] app.name");
  check(cfg["app"]["debug"].get<bool>() == false, "json [] debug");

  // operator() chaining
  check(cfg("app", "name").get<string>() == "json_test", "json () chain");
  check(cfg("database", "pool", "timeout").get<int>() == 60, "json () nested");

  // Write back
  cfg["app"]["version"] = 5;
  check(cfg["app"]["version"].get<int>() == 5, "json write back");

  // base class
  check(cfg.Filename() == path, "json Filename()");

  cout << "  ConfigJson all tests passed" << endl;
}

// ==================== ConfigRapidYml tests ====================

static void test_ryml_basic() {
  string path = string(getenv("HOME")) + "/software/config/git/test/config.yaml";

  ConfigRapidYml cfg(path);

  check(cfg.Verbose() == 2, "ryml Verbose == 2");
  check(cfg.Config()["app"]["name"].val() == "test_app", "ryml app.name");
  check(cfg.Config()["database"]["host"].val() == "localhost", "ryml db host");

  // operator[] access
  check(cfg["app"]["name"].val() == "test_app", "ryml [] app.name");

  // operator() chaining
  check(cfg("app", "name").val() == "test_app", "ryml () chain");

  // base class
  check(cfg.Filename() == path, "ryml Filename()");

  cout << "  ConfigRapidYml all tests passed" << endl;
}

// ==================== ConfigManager tests ====================

static void test_manager() {
  string yamlPath = string(getenv("HOME")) + "/software/config/git/test/config.yaml";
  string jsonPath = string(getenv("HOME")) + "/software/config/git/test/config.json";

  ConfigManager mgr;
  ConfigYAML yamlCfg(yamlPath);
  ConfigJson jsonCfg(jsonPath);

  mgr.Register("yaml", &yamlCfg);
  mgr.Register("json", &jsonCfg);

  check(mgr.Size() == 2, "manager has 2 entries");
  check(mgr.Has("yaml"), "manager has yaml");
  check(mgr.Has("json"), "manager has json");
  check(!mgr.Has("toml"), "manager has no toml");

  auto* base = mgr.Get("yaml");
  check(base != nullptr, "manager Get yaml");
  check(base->Verbose() == 2, "manager yaml Verbose");

  auto* typed = mgr.GetAs<ConfigYAML>("yaml");
  check(typed != nullptr, "manager GetAs yaml");
  check(typed->Config()["app"]["name"].as<string>() == "test_app", "manager yaml access");

  check(mgr.Get("nonexistent") == nullptr, "manager returns nullptr for missing");

  cout << "  ConfigManager all tests passed" << endl;
}

// ==================== main ====================

int main() {
  cout << "=== cxconfig unit tests ===" << endl;

  cout << "--- YAML ---" << endl;
  test_yaml_from_file();

  cout << "--- JSON ---" << endl;
  test_json_basic();

#if HAVE_TOML
  cout << "--- TOML ---" << endl;
  {
    string path = string(getenv("HOME")) + "/software/config/git/test/config.toml";
    ConfigTomlpp cfg(path);

    check(cfg.Verbose() == 2, "toml Verbose == 2");
    check(cfg["app"]["name"].value_or("") == std::string("toml_test"), "toml app.name");
    check(cfg["app"]["version"].value_or(0) == 2, "toml app.version");
    check(cfg["database"]["host"].value_or("") == std::string("toml-host"), "toml db host");
    check(cfg["database"]["pool"]["size"].value_or(0) == 5, "toml db pool size");
    check(cfg.Tree()["app"]["name"].value_or("") == std::string("toml_test"), "toml Tree() access");
    check(cfg["database"]["pool"]["size"].value_or(0) == 5, "toml [] chain db.pool.size");
    check(cfg["database"]["pool"]["timeout"].value_or(0) == 15, "toml [] chain db.pool.timeout");
    check(cfg["database"]["pool"]["tags"]["main"].value_or(false) == true, "toml [] chain db.pool.tags.main");
    check(cfg("app").is_table(), "toml () single key returns table node_view");
    check(cfg["count"].value_or(0) == 42, "toml int conversion");
    check(cfg["pi"].value_or(0.0) > 3.14, "toml float conversion");
    check(cfg["enabled"].value_or(false) == true, "toml bool conversion");
    check(cfg["empty_str"].value_or("default") == std::string(""), "toml empty string");
    check(cfg["mode"].value_or("") == std::string("test"), "toml string conversion");
    check(cfg["nonexistent"].value_or(-1) == -1, "toml missing key default");
    check(cfg["database"]["nonexistent"].value_or(99) == 99, "toml missing nested key default");
    check(cfg["app"]["missing_field"].value_or(0) == 0, "toml missing int default");
    auto seedNode = cfg["seed"];
    check(seedNode.is_array(), "toml seed is array");
    auto seedArr = seedNode.as_array();
    check(seedArr->size() == 3, "toml seed array size == 3");

    // base class
    check(cfg.Filename() == path, "toml Filename()");

    cout << "  ConfigTomlpp all tests passed" << endl;
  }
#else
  cout << "--- TOML: skipped (disabled) ---" << endl;
#endif

#if HAVE_RYML
  cout << "--- RYML ---" << endl;
  test_ryml_basic();
#else
  cout << "--- RYML: skipped (disabled) ---" << endl;
#endif

  cout << "--- Manager ---" << endl;
  test_manager();

  cout << endl << "ALL TESTS PASSED" << endl;
  return 0;
}
