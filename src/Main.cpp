#ifndef UNICODE
#define UNICODE
#endif

#include "app/AppHost.h"
#include "game-suite/fix_win32_compatibility.h"
#include "app/FontManager.h"
#include "tests/TestRunner.h"
#include <cstring>

Font appFont;

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#pragma comment(linker, "/STACK:" STR(CUSTOM_STACK_SIZE))
#pragma comment(linker, "/HEAP:200000000")

int main(int argc, char* argv[]) {
    if (argc > 1 && std::strcmp(argv[1], "--run-tests") == 0) {
        std::string testName = (argc > 2) ? argv[2] : "";
        return ::runOneTest(testName) ? 0 : 1;
    }

    AppHost host;
    host.run();
    return 0;
}
