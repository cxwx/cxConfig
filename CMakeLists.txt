cmake_minimum_required(VERSION 3.16)
project(cxconfig VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CXCONFIG_ENABLE_YAML "Build YAML backend" ON)
option(CXCONFIG_ENABLE_JSON "Build JSON backend" ON)
option(CXCONFIG_ENABLE_TOML "Build TOML backend" ON)
option(CXCONFIG_ENABLE_RYML "Build RYML backend" ON)
option(CXCONFIG_BUILD_TEST "Build test binary" ON)

set(CMAKE_INSTALL_PREFIX
    "$ENV{HOME}/software/install"
    CACHE PATH
    "Install prefix"
    FORCE
)

# --- yaml-cpp ---
if(CXCONFIG_ENABLE_YAML)
    find_package(yaml-cpp REQUIRED)
endif()

# --- nlohmann_json ---
if(CXCONFIG_ENABLE_JSON)
    find_package(nlohmann_json REQUIRED)
endif()

# --- toml++ ---
if(CXCONFIG_ENABLE_TOML)
    find_path(tomlplusplus_INCLUDE_DIRS toml++/toml.h)
    if(NOT tomlplusplus_INCLUDE_DIRS)
        message(
            FATAL_ERROR
            "toml++ not found. Install it or set CXCONFIG_ENABLE_TOML=OFF"
        )
    endif()
endif()

# --- ryml ---
if(CXCONFIG_ENABLE_RYML)
    find_package(ryml REQUIRED)
endif()

# === sources ===
set(CXCONFIG_SRCS)
set(CXCONFIG_PUBLIC_INCLUDES)
set(CXCONFIG_PUBLIC_LINKS)
set(CXCONFIG_PUBLIC_DEFS)

if(CXCONFIG_ENABLE_YAML)
    list(APPEND CXCONFIG_SRCS ConfigYAML.cc)
    list(APPEND CXCONFIG_PUBLIC_INCLUDES ${yaml-cpp_INCLUDE_DIRS})
    list(APPEND CXCONFIG_PUBLIC_LINKS yaml-cpp)
    list(APPEND CXCONFIG_PUBLIC_DEFS HAVE_YAML)
endif()

if(CXCONFIG_ENABLE_JSON)
    list(APPEND CXCONFIG_SRCS ConfigJson.cc)
    list(APPEND CXCONFIG_PUBLIC_INCLUDES ${nlohmann_json_INCLUDE_DIRS})
    list(APPEND CXCONFIG_PUBLIC_LINKS nlohmann_json::nlohmann_json)
    list(APPEND CXCONFIG_PUBLIC_DEFS HAVE_JSON)
endif()

if(CXCONFIG_ENABLE_TOML)
    list(APPEND CXCONFIG_SRCS ConfigTomlpp.cc)
    list(APPEND CXCONFIG_PUBLIC_INCLUDES ${tomlplusplus_INCLUDE_DIRS})
    list(APPEND CXCONFIG_PUBLIC_DEFS HAVE_TOML)
endif()

if(CXCONFIG_ENABLE_RYML)
    list(APPEND CXCONFIG_SRCS ConfigRapidYml.cc)
    list(APPEND CXCONFIG_PUBLIC_INCLUDES ${ryml_INCLUDE_DIRS})
    list(APPEND CXCONFIG_PUBLIC_LINKS ryml::ryml)
    list(APPEND CXCONFIG_PUBLIC_DEFS HAVE_RYML)
endif()

add_library(cxconfig SHARED ${CXCONFIG_SRCS})
set_target_properties(
    cxconfig
    PROPERTIES CXX_VISIBILITY_PRESET default POSITION_INDEPENDENT_CODE ON
)
add_library(cxconfig::cxconfig ALIAS cxconfig)

target_include_directories(
    cxconfig
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        ${CXCONFIG_PUBLIC_INCLUDES}
)
target_link_libraries(cxconfig PUBLIC ${CXCONFIG_PUBLIC_LINKS})
target_compile_definitions(cxconfig PUBLIC ${CXCONFIG_PUBLIC_DEFS})

# === test ===
if(CXCONFIG_BUILD_TEST)
    add_executable(cxconfig_test test/main.cpp)
    target_include_directories(
        cxconfig_test
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(cxconfig_test PRIVATE cxconfig::cxconfig)
    target_compile_definitions(
        cxconfig_test
        PRIVATE
            HAVE_YAML=$<BOOL:${CXCONFIG_ENABLE_YAML}>
            HAVE_JSON=$<BOOL:${CXCONFIG_ENABLE_JSON}>
            HAVE_TOML=$<BOOL:${CXCONFIG_ENABLE_TOML}>
            HAVE_RYML=$<BOOL:${CXCONFIG_ENABLE_RYML}>
    )
endif()

# === install ===
include(GNUInstallDirs)
install(
    TARGETS cxconfig
    EXPORT cxconfigTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    FILES
        ConfigBase.hh
        ConfigManager.hh
        ConfigYAML.hh
        ConfigJson.hh
        ConfigTomlpp.hh
        ConfigRapidYml.hh
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc
)
install(
    EXPORT cxconfigTargets
    NAMESPACE cxconfig::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxconfig
)
