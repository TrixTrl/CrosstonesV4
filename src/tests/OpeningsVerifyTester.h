#pragma once
#include "TestBase.h"
#include <string>

// run with `ctest -R OpeningsVerifyTester` after editing openings.txt.
class OpeningsVerifyTester : public TestBase {
public:
    const char* name() const override { return "OpeningsVerifyTester"; }
    bool run() override;
    const char* error() const override { return error_.c_str(); }

private:
    std::string error_;
};
