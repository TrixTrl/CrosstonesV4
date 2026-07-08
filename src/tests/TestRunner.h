#pragma once
#include <string>
#include <vector>

// Run a single test by name, or all if name is empty.
bool runOneTest(const std::string& name);

// Return all registered test names.
std::vector<std::string> allTestNames();
