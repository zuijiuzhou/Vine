#include <vine/co/Task.hpp>
#include <vine/co/AsyncEvent.hpp>
#include <vine/co/Awaitable.hpp>
#include <vine/co/Cancellation.hpp>
#include <vine/co/DetachedTask.hpp>
#include <vine/co/Scheduler.hpp>
#include <vine/co/SyncWait.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>
#include <thread>

using namespace vine;

namespace {

co::Task<int> answer()
{
    co_return 42;
}

co::Task<void> noop()
{
    co_return;
}

co::Task<int> addOne(int x)
{
    co_return x + 1;
}

co::Task<int> nested()
{
    int a = co_await answer();
    int b = co_await addOne(a);
    co_return b;
}

co::Task<int> failing()
{
    throw std::runtime_error("boom");
    co_return 1;
}

co::Task<int> lazyProbe(bool& ran)
{
    ran = true;
    co_return 7;
}

} // namespace

TEST(TaskTest, SyncWaitReturnsValue)
{
    EXPECT_EQ(co::syncWait(answer()), 42);
    EXPECT_EQ(co::syncWait(nested()), 43);
}

TEST(TaskTest, SyncWaitVoid)
{
    co::syncWait(noop());
    SUCCEED();
}

TEST(TaskTest, LazySemantics)
{
    bool ran = false;
    auto task = lazyProbe(ran);
    EXPECT_FALSE(ran);
    EXPECT_EQ(co::syncWait(std::move(task)), 7);
    EXPECT_TRUE(ran);
}

TEST(TaskTest, ExceptionPropagates)
{
    EXPECT_THROW(co::syncWait(failing()), std::runtime_error);
}

TEST(TaskTest, MoveSemantics)
{
    auto task = answer();
    auto moved = std::move(task);
    EXPECT_FALSE(static_cast<bool>(task));
    EXPECT_TRUE(static_cast<bool>(moved));
    EXPECT_EQ(co::syncWait(std::move(moved)), 42);
}

TEST(DetachedTaskTest, RunsEagerly)
{
    co::AsyncEvent event;
    auto task = [&event]() -> co::DetachedTask {
        event.set();
        co_return;
    }();
    EXPECT_TRUE(event.isSet());
}

TEST(AsyncEventTest, SetResumesWaiter)
{
    co::AsyncEvent event;

    auto task = [&event]() -> co::Task<int> {
        co_await event;
        co_return 99;
    }();

    std::thread setter([&event] {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        event.set();
    });

    EXPECT_EQ(co::syncWait(std::move(task)), 99);
    setter.join();
}

TEST(SchedulerTest, InlineSchedulerRunsInline)
{
    co::InlineScheduler scheduler;
    auto task = [&]() -> co::Task<int> {
        co_await scheduler.schedule();
        co_return 1;
    }();
    EXPECT_EQ(co::syncWait(std::move(task)), 1);
}

TEST(SchedulerTest, ScheduleOnRunsTask)
{
    co::InlineScheduler scheduler;
    auto task = co::scheduleOn(scheduler, answer());
    EXPECT_EQ(co::syncWait(std::move(task)), 42);
}

TEST(CancellationTest, TokenAliases)
{
    co::CancellationSource source;
    co::CancellationToken token = source.get_token();
    EXPECT_FALSE(token.stop_requested());
    source.request_stop();
    EXPECT_TRUE(token.stop_requested());
}
