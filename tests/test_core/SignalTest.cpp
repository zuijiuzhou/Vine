#include <gtest/gtest.h>
#include <vine/Signal.hpp>

#include <string>
#include <vector>

TEST(Signal, RvalueEmitDeliveredToAllHandlers)
{
    vine::Signal<std::string> signal;

    std::string first;
    std::string second;

    signal.addHandler([&](std::string value) {
        first = value;
    });

    signal.addHandler([&](std::string value) {
        second = value;
    });

    signal.emit(std::string("payload"));

    EXPECT_EQ(first, "payload");
    EXPECT_EQ(second, "payload");
}

TEST(Signal, RemoveSelfDuringEmitIsSafe)
{
    vine::Signal<int> signal;
    std::vector<int> called;

    vine::Signal<int>::HandlerId self_id = 0;
    self_id = signal.addHandler([&](int value) {
        called.push_back(value * 10);
        signal.removeHandler(self_id);
    });

    signal.addHandler([&](int value) {
        called.push_back(value);
    });

    signal.emit(3);
    signal.emit(4);

    const std::vector<int> expected{ 30, 3, 4 };
    EXPECT_EQ(called, expected);
}

TEST(Signal, AddDuringEmitTakesEffectNextEmit)
{
    vine::Signal<int> signal;
    std::vector<int> called;

    signal.addHandler([&](int value) {
        called.push_back(value);
        signal.addHandler([&](int next_value) {
            called.push_back(next_value * 100);
        });
    });

    signal.emit(1);
    signal.emit(2);

    const std::vector<int> expected{ 1, 2, 200 };
    EXPECT_EQ(called, expected);
}

TEST(Signal, BlockedSignalDoesNotEmit)
{
    vine::Signal<int> signal;
    int               called_count = 0;

    signal.addHandler([&](int) {
        ++called_count;
    });

    signal.setBlocked(true);
    signal.emit(42);

    EXPECT_EQ(called_count, 0);
    EXPECT_TRUE(signal.isBlocked());

    signal.setBlocked(false);
    signal.emit(42);

    EXPECT_EQ(called_count, 1);
}
