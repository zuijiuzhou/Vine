// Reproduce on godbolt.org: paste this whole file, pick a compiler below,
// and enable the "Execute the code" pane (the ⏵ / "Execute" button).

// This demonstrates USER-SIDE UB, NOT a compiler bug:
// an immediately-invoked coroutine lambda reads its captures after the
// temporary closure object was destroyed at the end of the full expression.
// See cppreference "Coroutines" → bad3/good, and docs/coroutine-lambda-capture-bug.md.

// Compiler: any of gcc 15/16, clang 22/23   → prints "lambda : 4 4 4 4" or SIGSEGV
// Flags:    -std=c++20 -O1    (also try -O0, -O2)
// Observed: "lambda : 4 4 4 4" (or SIGSEGV)  and  "named : 1 2 3 4"
// The "lambda" block is UB; the "named" block is correct (parameters are
// copied into the coroutine state).

#include <coroutine>
#include <cstdio>
#include <vector>

struct Task {
    struct promise_type {
        Task get_return_object() {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() noexcept {}
    };

    explicit Task(std::coroutine_handle<promise_type> h) noexcept : h_(h) {}
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& o) noexcept : h_(o.h_) { o.h_ = nullptr; }
    Task& operator=(Task&& o) noexcept
    {
        if (this != &o)
        {
            if (h_) h_.destroy();
            h_ = o.h_;
            o.h_ = nullptr;
        }
        return *this;
    }
    ~Task() { if (h_) h_.destroy(); }

    std::coroutine_handle<promise_type> h_;
};

// Control: a plain (named) coroutine function — this one is CORRECT.
static Task named(std::vector<int>& order, int i)
{
    order.push_back(i);
    co_return;
}

int main()
{
    // UB: immediately-invoked coroutine lambda; the closure temporary dies at
    // the end of the full expression, resume() then reads destroyed captures.
    {
        std::vector<int> order;
        std::vector<Task> tasks;
        for (int i = 1; i <= 4; ++i)
            tasks.push_back([&order, i]() -> Task { order.push_back(i); co_return; }());
        for (auto& t : tasks) t.h_.resume();
        std::printf("lambda : ");
        for (int x : order) std::printf("%d ", x);
        std::printf("\n");
    }

    // Control: named coroutine function with the same by-value parameter.
    {
        std::vector<int> order;
        std::vector<Task> tasks;
        for (int i = 1; i <= 4; ++i)
            tasks.push_back(named(order, i));
        for (auto& t : tasks) t.h_.resume();
        std::printf("named  : ");
        for (int x : order) std::printf("%d ", x);
        std::printf("\n");
    }

    return 0;
}
