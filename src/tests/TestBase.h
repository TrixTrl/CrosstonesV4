#pragma once
#include <string>
#include <functional>

class TestBase {
public:
    virtual ~TestBase() = default;
    virtual const char* name() const = 0;
    virtual bool run() = 0;
    virtual const char* error() const = 0;
};

using TestFactory = std::function<TestBase*()>;

// Register a test factory under a name
void registerTest(const char* name, TestFactory factory);

#define REGISTER_TEST(ClassName)                                         \
    static struct ClassName##_Reg_ {                                     \
        ClassName##_Reg_() {                                             \
            ::registerTest(#ClassName,                                   \
                []() -> TestBase* { return new ClassName(); });          \
        }                                                                \
    } ClassName##_reg_;
