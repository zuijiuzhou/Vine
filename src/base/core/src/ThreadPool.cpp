#include <vine/ThreadPool.hpp>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

V_CORE_NS_BEGIN

struct ThreadPool::Impl {
    std::vector<std::thread>          workers;
    std::queue<std::function<void()>> tasks;
    std::mutex                        mutex;
    std::condition_variable           cv;
    bool                              stop = false;
};

ThreadPool::ThreadPool(std::size_t thread_count)
  : d(new Impl)
{
    if (thread_count == 0) {
        thread_count = 1;
    }
    d->workers.reserve(thread_count);
    for (std::size_t i = 0; i < thread_count; ++i) {
        d->workers.emplace_back([this]() {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(d->mutex);
                    d->cv.wait(lock, [this]() { return d->stop || !d->tasks.empty(); });
                    if (d->stop && d->tasks.empty()) {
                        return;
                    }
                    task = std::move(d->tasks.front());
                    d->tasks.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(d->mutex);
        d->stop = true;
    }
    d->cv.notify_all();
    for (auto& worker : d->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    delete d;
}

void ThreadPool::enqueueTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(d->mutex);
        if (d->stop) {
            return;
        }
        d->tasks.push(std::move(task));
    }
    d->cv.notify_one();
}

std::size_t ThreadPool::threadCount() const
{
    return d->workers.size();
}

V_CORE_NS_END
