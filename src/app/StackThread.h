#pragma once

#include <windows.h>
#include <functional>
#include <utility>

// 64MB — keep below 32-bit signed integer limit
#define CUSTOM_STACK_SIZE 67108864

// Thread wrapper that uses CreateThread with a custom stack reservation.
// Drop-in replacement for std::thread when large stack is needed.
class StackThread {
public:
    template<typename F>
    explicit StackThread(F func, size_t stackSize = CUSTOM_STACK_SIZE) {
        auto wrapped = new std::function<void()>(std::move(func));
        threadHandle = CreateThread(
            NULL,
            stackSize,
            [](LPVOID param) -> DWORD {
                auto f = static_cast<std::function<void()>*>(param);
                (*f)();
                delete f;
                return 0;
            },
            wrapped,
            STACK_SIZE_PARAM_IS_A_RESERVATION,
            &threadId
        );
    }

    ~StackThread() {
        if (joinable()) detach();
    }

    StackThread(const StackThread&) = delete;
    StackThread& operator=(const StackThread&) = delete;

    StackThread(StackThread&& other) noexcept
        : threadHandle(other.threadHandle), threadId(other.threadId) {
        other.threadHandle = NULL;
    }

    StackThread& operator=(StackThread&& other) noexcept {
        if (this != &other) {
            if (joinable()) detach();
            threadHandle = other.threadHandle;
            threadId = other.threadId;
            other.threadHandle = NULL;
        }
        return *this;
    }

    bool joinable() const { return threadHandle != NULL; }

    void join() {
        if (threadHandle) {
            WaitForSingleObject(threadHandle, INFINITE);
            CloseHandle(threadHandle);
            threadHandle = NULL;
        }
    }

    void detach() {
        if (threadHandle) {
            CloseHandle(threadHandle);
            threadHandle = NULL;
        }
    }

private:
    HANDLE threadHandle = NULL;
    DWORD threadId = 0;
};
