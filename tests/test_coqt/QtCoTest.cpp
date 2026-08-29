#include <vine/co/Task.hpp>
#include <vine/co/DetachedTask.hpp>
#include <vine/co/SyncWait.hpp>
#include <vine/co/Cancellation.hpp>

#include "co/Scheduler.hpp"
#include "co/Sleep.hpp"

#include <QCoreApplication>
#include <QTimer>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>

using namespace vine;

namespace {

class QtCoTest : public ::testing::Test
{
  protected:
    static void SetUpTestSuite()
    {
        static int argc = 1;
        static char arg0[] = "test_coqt";
        static char* argv[] = { arg0, nullptr };
        app_ = std::make_unique<QCoreApplication>(argc, argv);
    }

    static void TearDownTestSuite()
    {
        app_.reset();
    }

    static std::unique_ptr<QCoreApplication> app_;
};

std::unique_ptr<QCoreApplication> QtCoTest::app_;

} // namespace

TEST_F(QtCoTest, SchedulerResumesOnOwningThread)
{
    appfw::co::Scheduler scheduler;
    bool ran = false;

    auto task = [&]() -> co::DetachedTask {
        co_await scheduler.schedule();
        ran = true;
        QCoreApplication::quit();
        co_return;
    }();

    QTimer::singleShot(5000, [] { QCoreApplication::quit(); });
    QCoreApplication::instance()->exec();

    EXPECT_TRUE(ran);
}

TEST_F(QtCoTest, SleepRunsOnTimer)
{
    bool done = false;
    auto start = std::chrono::steady_clock::now();

    auto task = [&]() -> co::DetachedTask {
        co_await appfw::co::sleep(std::chrono::milliseconds(50));
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

TEST_F(QtCoTest, SleepCancellationResumesEarly)
{
    co::CancellationSource source;
    bool cancelled = false;

    auto task = [&]() -> co::DetachedTask {
        try
        {
            co_await appfw::co::sleep(std::chrono::milliseconds(1000), source.get_token());
        }
        catch (const co::OperationCanceled&)
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
