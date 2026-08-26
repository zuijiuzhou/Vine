#pragma once
#include "core_global.hpp"

#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <type_traits>
#include <utility>

V_CORE_NS_BEGIN

/**
 * @brief A fixed-size thread pool that executes queued tasks concurrently.
 *
 * Tasks are submitted with enqueue() and may return a result through a
 * std::future. Worker threads are started by the constructor and joined by
 * the destructor, so the pool must outlive any task it runs. The pool is not
 * copyable or movable.
 *
 * Qt-free; relies only on the C++ standard library.
 */
class V_CORE_API ThreadPool {

  public:
    /**
     * @brief Creates a thread pool with the given number of worker threads.
     *
     * @param thread_count Number of worker threads; clamped to at least 1.
     */
    explicit ThreadPool(std::size_t thread_count);

    /**
     * @brief Destroys the pool, stops accepting tasks and joins all workers.
     */
    ~ThreadPool();

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&)                 = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

  public:
    /**
     * @brief Queues a callable for execution on a worker thread.
     *
     * @tparam F Callable type.
     * @tparam Args Argument types.
     * @param f Callable to invoke on a worker thread.
     * @param args Arguments forwarded to f.
     * @return A future that yields the result of invoking f.
     */
    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

    /**
     * @brief Returns the number of worker threads.
     *
     * @return The worker thread count.
     */
    std::size_t threadCount() const;

  private:
    /**
     * @brief Pushes a packaged task onto the shared queue.
     *
     * @param task Callable to run on a worker thread.
     */
    void enqueueTask(std::function<void()> task);

    struct Impl;
    Impl* const d;
};

template <typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
{
    using ResultType = std::invoke_result_t<F, Args...>;
    auto task        = std::make_shared<std::packaged_task<ResultType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto result      = task->get_future();
    enqueueTask([task]() { (*task)(); });
    return result;
}

V_CORE_NS_END
