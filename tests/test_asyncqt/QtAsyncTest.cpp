#include <vine/async/Task.hpp>
#include <vine/async/DetachedTask.hpp>
#include <vine/async/SyncWait.hpp>
#include <vine/async/Cancellation.hpp>

#include <vine/CancellationToken.hpp>

#include "async/Scheduler.hpp"
#include "async/Sleep.hpp"

#include <QCoreApplication>
#include <QTimer>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>

using namespace vine;

namespace {

class QtAsyncTest : public ::testing::Test
{
  protected:
    static void SetUpTestSuite()
    {
        static int argc = 1;
        static char arg0[] = "test_asyncqt";
        static char* argv[] = { arg0, nullptr };
        app_ = std::make_unique<QCoreApplication>(argc, argv);
    }

    static void TearDownTestSuite()
    {
        app_.reset();
    }

    static std::unique_ptr<QCoreApplication> app_;
};

std::unique_ptr<QCoreApplication> QtAsyncTest::app_;

} // namespace

TEST_F(QtAsyncTest, SchedulerResumesOnOwningThread)
{
    appfw::async::Scheduler scheduler;
    bool ran = false;

    auto task = [&]() -> async::DetachedTask {
        co_await scheduler.schedule();
        ran = true;
        QCoreApplication::quit();
        co_return;
    }();

    QTimer::singleShot(5000, [] { QCoreApplication::quit(); });
    QCoreApplication::instance()->exec();

    EXPECT_TRUE(ran);
}

TEST_F(QtAsyncTest, SleepRunsOnTimer)
{
    bool done = false;
    auto start = std::chrono::steady_clock::now();

    auto task = [&]() -> async::DetachedTask {
        co_await appfw::async::sleep(std::chrono::milliseconds(50));
        done = true;
        QCoreApplication::quit();
        co_return;
    }();

    QTimer::singleShot(5000, [] { QCoreApplication::quit(); });
    QCoreApplication::instance()->exec();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    EXPECT_TRUE(done);
    EXPECT_GE(elapsed.count(), 40);
}

TEST_F(QtAsyncTest, SleepCancellationResumesEarly)
{
    vine::CancellationSource source;
    bool cancelled = false;

    auto task = [&]() -> async::DetachedTask {
        try
        {
            co_await appfw::async::sleep(std::chrono::milliseconds(1000), source.get_token());
        }
        catch (const async::TaskCancelledException&)
        {
            cancelled = true;
        }
        QCoreApplication::quit();
        co_return;
    }();

    QTimer::singleShot(50, [&] { source.request_stop(); });
    QTimer::singleShot(5000, [] { QCoreApplication::quit(); });
    QCoreApplication::instance()->exec();

    EXPECT_TRUE(cancelled);
}
