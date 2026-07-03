#pragma once
#include <memory>
#include <string_view>
#include <vector>
#include <unordered_map>
#include "Config.h"

class App;

class AppRegistry {
public:
    using Factory = std::function<std::unique_ptr<App>()>;

    struct Entry {
        AppMode mode;
        std::string_view name;
        std::string_view description;
        Factory factory;
    };

    static void registerApp(AppMode mode, std::string_view name,
                            std::string_view description, Factory factory) {
        auto& entries = allEntries();
        byMode()[mode] = entries.size();
        entries.push_back({mode, name, description, std::move(factory)});
    }

    static std::unique_ptr<App> create(AppMode mode) {
        auto it = byMode().find(mode);
        if (it != byMode().end()) {
            auto& entries = allEntries();
            if (it->second < entries.size())
                return entries[it->second].factory();
        }
        return nullptr;
    }

    static const std::vector<Entry>& entries() {
        return allEntries();
    }

private:
    static std::vector<Entry>& allEntries() {
        static std::vector<Entry> entries;
        return entries;
    }
    static std::unordered_map<AppMode, size_t>& byMode() {
        static std::unordered_map<AppMode, size_t> map;
        return map;
    }
};

#define REGISTER_APP(ModeEnum, ClassName, DisplayName, Description)   \
    namespace {                                                       \
        struct Register##ClassName {                                  \
            Register##ClassName() {                                   \
                AppRegistry::registerApp(                             \
                    AppMode::ModeEnum, DisplayName, Description,      \
                    []() { return std::make_unique<ClassName>(); }    \
                );                                                    \
            }                                                         \
        } reg_##ClassName;                                            \
    }

// Central registration — called from Main.cpp before using the registry.
// Add new apps here instead of using the REGISTER_APP macro (which relies
// on static init order that doesn't work reliably with MSVC static libs).
void registerAllApps();
