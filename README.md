# cxconfig

C++17 多格式配置文件解析库。统一抽象层，支持 YAML、JSON、TOML、RYML 四种后端。

## 设计原则

Speed is everything.

## 后端对比

| 维度 | yaml-cpp | nlohmann_json | toml++ | rapidyaml |
|------|----------|---------------|--------|-----------|
| 速度 | 最慢 | 快 | 快 | 最快 |
| 易用性 | 最简单 | 简单 | 简单 | 较难 |
| 写入 | 支持(引用语义) | 支持(引用语义) | 需手动 | 需手动 |
| 头文件开销 | 轻 | 重(模板密集) | 重 | 中 |
| CMake | find_package | find_package | find_path | find_package |

## 接口对比

### 获取根节点

| 后端 | 方法 | 返回类型 |
|------|------|----------|
| YAML | `Config()` | `YAML::Node` (值拷贝) |
| JSON | `Config()` | `const nlohmann::json&` (const引用) |
| TOML | `Tree()` | `const toml::table&` (const引用) |
| RYML | `Config()` / `Tree()` | `ryml::ConstNodeRef` / `ryml::Tree` (值拷贝) |

### operator[] 单键访问

| 后端 | 返回类型 | const重载 |
|------|----------|-----------|
| YAML | `YAML::Node` | 仅const |
| JSON | `nlohmann::json&` | 有const和非const |
| TOML | `toml::node_view<const toml::node>` | 仅const |
| RYML | `ryml::ConstNodeRef` | 仅const |

### operator() 链式访问

| 后端 | 支持 | 返回类型 | 示例 |
|------|------|----------|------|
| YAML | 支持 | `YAML::Node` | `cfg("db", "host")` |
| JSON | 支持 | `const nlohmann::json&` | `cfg("db", "host")` |
| TOML | **不支持** | - | - |
| RYML | 支持 | `ryml::ConstNodeRef` | `cfg("db", "host")` |

### 读取值的方式

| 后端 | API | 示例 |
|------|-----|------|
| YAML | `node.as<T>()` | `cfg["app"]["name"].as<string>()` |
| JSON | `node.get<T>()` | `cfg["app"]["name"].get<string>()` |
| TOML | `node.value_or(default)` | `cfg["app"]["name"].value_or("")` |
| RYML | `node >> var` 或 `node.val()` | `cfg["app"]["name"].val()` |

### 写入值

| 后端 | 方式 | 示例 |
|------|------|------|
| YAML | `node = value` | `cfg["app"]["version"] = 4` |
| JSON | `node = value` | `cfg["app"]["version"] = 5` |
| TOML | 不直接支持 | 需通过 `Tree()` 获取 table 后修改 |
| RYML | 不直接支持 | 需获取可变 `NodeRef` |

## 能否统一接口？

**结论：不建议完全统一。**

各后端返回类型完全不同（`YAML::Node` vs `nlohmann::json&` vs `toml::node_view` vs `ryml::ConstNodeRef`），这是各库自身的设计。强行统一会：

1. **增加复杂度** — 需要包装器/variant，增加间接层
2. **降低性能** — 额外拷贝或堆分配，违背 "Speed is everything" 原则
3. **失去类型安全** — 用户无法利用底层库的编译期检查
4. **降低灵活性** — 无法直接使用底层库的丰富 API

**可行的折中方案：**

如果确实需要统一接口，可以提供一个轻量级的 `ConfigValue` 包装器，仅暴露常用操作：

```cpp
class ConfigValue {
  std::variant<
    YAML::Node,
    const nlohmann::json*,
    toml::node_view<const toml::node>,
    ryml::ConstNodeRef
  > data;
public:
  template<typename T> T as() const;  // 统一读取接口
};
```

但这会引入 `std::variant` 的运行时开销和类型擦除，对性能敏感场景不友好。

**当前设计的合理性：**

当前设计让用户直接使用底层库 API，学习成本低（用户已熟悉底层库），且无额外开销。`operator[]` 和 `operator()` 提供了统一的访问语法，差异仅在读取值的方法上。

## 构建

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

### CMake 选项

| 选项 | 默认 | 说明 |
|------|------|------|
| `CXCONFIG_ENABLE_YAML` | ON | 启用 yaml-cpp 后端 |
| `CXCONFIG_ENABLE_JSON` | ON | 启用 nlohmann_json 后端 |
| `CXCONFIG_ENABLE_TOML` | ON | 启用 toml++ 后端 |
| `CXCONFIG_ENABLE_RYML` | ON | 启用 rapidyaml 后端 |
| `CXCONFIG_BUILD_TEST` | ON | 构建测试 |

禁用不需要的后端可显著减少编译时间（尤其是 nlohmann_json）。

## 依赖

通过 Homebrew 安装：

```bash
brew install yaml-cpp nlohmann-json tomlplusplus rapidyaml
```

## 使用

```cpp
#include "ConfigYAML.hh"
#include "ConfigJson.hh"

using namespace cxfunc::config;

// YAML
ConfigYAML yaml("config.yaml");
string name = yaml["app"]["name"].as<string>();
string host = yaml("database", "host").as<string>();  // 链式访问

// JSON
ConfigJson json("config.json");
string name = json["app"]["name"].get<string>();
int timeout = json("database", "pool", "timeout").get<int>();  // 链式访问
```
