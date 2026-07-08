#include "TestRunner.h"
#include "TestBase.h"
#include <iostream>
#include <map>
#include <mutex>

// ── registry (function-local static for init-order safety) ──

static std::map<std::string, TestFactory>& registry() {
    static std::map<std::string, TestFactory> reg;
    return reg;
}
static std::mutex& registryMutex() {
    static std::mutex mtx;
    return mtx;
}

void registerTest(const char* name, TestFactory factory) {
    std::lock_guard<std::mutex> lock(registryMutex());
    registry()[name] = std::move(factory);
}

// ── runner ──

bool runOneTest(const std::string& name) {
    std::lock_guard<std::mutex> lock(registryMutex());
    bool allOk = true;
    bool found = false;
    for (auto& [testName, factory] : registry()) {
        if (!name.empty() && testName != name) continue;
        found = true;
        auto test = factory();
        bool ok = test->run();
        std::cout << "  [" << (ok ? "PASS" : "FAIL") << "] " << testName << "\n";
        if (!ok) {
            std::string err = test->error();
            if (!err.empty())
                std::cout << "    " << err << "\n";
        }
        allOk &= ok;
        delete test;
    }
    if (!found && !name.empty()) {
        std::cout << "  [SKIP] Unknown test \"" << name << "\"\n";
        return false;
    }
    return allOk;
}

std::vector<std::string> allTestNames() {
    std::lock_guard<std::mutex> lock(registryMutex());
    std::vector<std::string> names;
    for (auto& [n, _] : registry())
        names.push_back(n);
    return names;
}
