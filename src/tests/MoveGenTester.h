#pragma once
#include "TestBase.h"
#include <string>

class MoveGenTester : public TestBase {
public:
    const char* name() const override { return "MoveGenTester"; }
    bool run() override;
    const char* error() const override { return error_.c_str(); }

private:
    std::string error_;
};
