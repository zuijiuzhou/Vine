// EventBus 单元测试：Object 派生事件、多态派发、线程模式、RAII。
//
// 覆盖：发布/订阅、多态（订阅基类收派生事件）、RAII 自动退订、move 转移、
// 派发中退订安全性、异常隔离、Current/Main/Auto 线程模式、异步重校验。

#include <gtest/gtest.h>

#include <QCoreApplication>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/EventBus.hpp>
#include <vine/Events.hpp>

#include <atomic>
#include <memory>
#include <stdexcept>
#include <thread>

namespace
{

class BaseEvent : public vine::EventArgs {
  public:
    V_OBJECT_META_DECL;
};
V_OBJECT_META_IMPL(BaseEvent, vine::EventArgs)

class DerivedEvent : public BaseEvent {
  public:
    V_OBJECT_META_DECL;
};
V_OBJECT_META_IMPL(DerivedEvent, BaseEvent)

class PingEvent : public vine::EventArgs {
  public:
    V_OBJECT_META_DECL;
};
V_OBJECT_META_IMPL(PingEvent, vine::EventArgs)

} // namespace

TEST(EventBusTest, PublishDeliversToSubscribers)
{
    vine::appfw::EventBus bus;
    int                   received = 0;
    auto                  sub      = bus.subscribe<PingEvent>([&](const PingEvent&) { ++received; });
    bus.publish(std::make_shared<PingEvent>());
    bus.publish(std::make_shared<PingEvent>());
    EXPECT_EQ(received, 2);
}

TEST(EventBusTest, DifferentTypesAreIsolated)
{
    vine::appfw::EventBus bus;
    int                   ping = 0;
    int                   base = 0;
    auto                  s1   = bus.subscribe<PingEvent>([&](const PingEvent&) { ++ping; });
    auto                  s2   = bus.subscribe<BaseEvent>([&](const BaseEvent&) { ++base; });
    bus.publish(std::make_shared<PingEvent>());
    EXPECT_EQ(ping, 1);
    EXPECT_EQ(base, 0);  // PingEvent 与 BaseEvent 无继承关系，互不派发
}

TEST(EventBusTest, PolymorphicDispatch)
{
    vine::appfw::EventBus bus;
    int                   base    = 0;
    int                   derived = 0;
    auto                  s1      = bus.subscribe<BaseEvent>([&](const BaseEvent&) { ++base; });
    auto                  s2      = bus.subscribe<DerivedEvent>([&](const DerivedEvent&) { ++derived; });

    bus.publish(std::make_shared<DerivedEvent>());
    EXPECT_EQ(derived, 1);  // 精确类型订阅者收到
    EXPECT_EQ(base, 1);     // 订阅基类也收到派生事件

    bus.publish(std::make_shared<BaseEvent>());
    EXPECT_EQ(derived, 1);  // 发布基类，派生订阅者不收
    EXPECT_EQ(base, 2);
}

TEST(EventBusTest, PublishWithNoSubscribersIsNoOp)
{
    vine::appfw::EventBus bus;
    EXPECT_NO_THROW(bus.publish(std::make_shared<PingEvent>()));
}

TEST(EventBusTest, SubscriptionUnsubscribesOnDestruction)
{
    vine::appfw::EventBus bus;
    int                   received = 0;
    {
        auto sub = bus.subscribe<PingEvent>([&](const PingEvent&) { ++received; });
        bus.publish(std::make_shared<PingEvent>());
        EXPECT_EQ(received, 1);
    }
    bus.publish(std::make_shared<PingEvent>());
    EXPECT_EQ(received, 1);  // RAII 自动退订
}

TEST(EventBusTest, MoveTransfersOwnership)
{
    vine::appfw::EventBus bus;
    int                   received = 0;
    auto                  sub      = bus.subscribe<PingEvent>([&](const PingEvent&) { ++received; });
    auto                  sub2     = std::move(sub);
    EXPECT_FALSE(sub.isActive());
    EXPECT_TRUE(sub2.isActive());
    bus.publish(std::make_shared<PingEvent>());
    EXPECT_EQ(received, 1);

    sub2.unsubscribe();
    EXPECT_FALSE(sub2.isActive());
    bus.publish(std::make_shared<PingEvent>());
    EXPECT_EQ(received, 1);
}

TEST(EventBusTest, UnsubscribeInsideHandlerIsSafe)
{
    vine::appfw::EventBus bus;
    int                   first  = 0;
    int                   second = 0;
    std::shared_ptr<vine::appfw::Subscription> s1;
    std::shared_ptr<vine::appfw::Subscription> s2;
    s1 = std::make_shared<vine::appfw::Subscription>(
        bus.subscribe<PingEvent>([&](const PingEvent&) {
            ++first;
            if (s2) {
                s2->unsubscribe();
            }
        }));
    s2 = std::make_shared<vine::appfw::Subscription>(
        bus.subscribe<PingEvent>([&](const PingEvent&) {
            ++second;
        }));

    ASSERT_NO_THROW(bus.publish(std::make_shared<PingEvent>()));
    // CopyOnWrite: the dispatch list is fixed at publish start, so s2 still
    // runs this round even though s1 unsubscribed it mid-dispatch.
    EXPECT_EQ(first, 1);
    EXPECT_EQ(second, 1);

    // The unsubscribe only affects the next publish.
    bus.publish(std::make_shared<PingEvent>());
    EXPECT_EQ(first, 2);
    EXPECT_EQ(second, 1);
}

TEST(EventBusTest, SubscriberExceptionIsContained)
{
    vine::appfw::EventBus bus;
    int                   received = 0;
    auto                  s1       = bus.subscribe<PingEvent>([](const PingEvent&) { throw std::runtime_error("boom"); });
    auto                  s2       = bus.subscribe<PingEvent>([&](const PingEvent&) { ++received; });
    ASSERT_NO_THROW(bus.publish(std::make_shared<PingEvent>()));
    EXPECT_EQ(received, 1);  // remaining subscribers still run
}

TEST(EventBusTest, MainModeQueuesDelivery)
{
    vine::appfw::EventBus bus;
    ASSERT_NE(vine::appfw::Application::current(), nullptr);  // GuiEnv 提供 Application
    int                   received = 0;
    auto                  sub      = bus.subscribe<PingEvent>([&](const PingEvent&) { ++received; },
                                        vine::appfw::SubscriptionThreadMode::Main);

    bus.publish(std::make_shared<PingEvent>());
    EXPECT_EQ(received, 0);  // 未立即执行

    QCoreApplication::processEvents();  // 冲刷主线程投递队列
    EXPECT_EQ(received, 1);
}

TEST(EventBusTest, AutoModeOnMainIsSynchronous)
{
    vine::appfw::EventBus bus;
    int                   received = 0;
    auto                  sub      = bus.subscribe<PingEvent>([&](const PingEvent&) { ++received; },
                                        vine::appfw::SubscriptionThreadMode::Auto);
    // 测试线程即主线程 → Auto → Current（同步）
    bus.publish(std::make_shared<PingEvent>());
    EXPECT_EQ(received, 1);
}

TEST(EventBusTest, AutoModeOffMainQueuesToMain)
{
    vine::appfw::EventBus bus;
    std::atomic<int>      received{ 0 };
    auto                  sub = bus.subscribe<PingEvent>([&](const PingEvent&) { ++received; },
                                        vine::appfw::SubscriptionThreadMode::Auto);
    // 非主线程发布 → Auto → Main（排队到主线程）
    std::thread worker([&] { bus.publish(std::make_shared<PingEvent>()); });
    worker.join();
    EXPECT_EQ(received.load(), 0);
    QCoreApplication::processEvents();
    EXPECT_EQ(received.load(), 1);
}

TEST(EventBusTest, UnsubscribedBeforeQueuedDeliveryIsSkipped)
{
    vine::appfw::EventBus bus;
    int                   received = 0;
    auto                  sub      = bus.subscribe<PingEvent>([&](const PingEvent&) { ++received; },
                                        vine::appfw::SubscriptionThreadMode::Main);
    bus.publish(std::make_shared<PingEvent>());
    sub.unsubscribe();  // 主线程投递前退订
    QCoreApplication::processEvents();
    EXPECT_EQ(received, 0);  // 异步重校验跳过
}

TEST(EventBusTest, EventOutlivesPublishViaSharedPtr)
{
    vine::appfw::EventBus bus;
    std::weak_ptr<const PingEvent> weak;
    {
        auto event = std::make_shared<PingEvent>();
        weak       = event;
        auto sub = bus.subscribe<PingEvent>([](const PingEvent&) {},
                                            vine::appfw::SubscriptionThreadMode::Main);
        bus.publish(event);
        event.reset();
        EXPECT_FALSE(weak.expired());  // 排队任务持有 shared_ptr，事件存活
    }
    QCoreApplication::processEvents();
    EXPECT_TRUE(weak.expired());  // 投递完成后释放
}
