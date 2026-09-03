// Unit tests for the Python-free event registry (scripting/Events.h). The
// pybind11 bridge (modules/Events.h) and the game-side dispatch (ProcessEvent)
// need the game and are not unit tested; the registry logic is.
#include <gtest/gtest.h>

#include <stdexcept>

#include "Events.h"

namespace {

using Events::Type;

class EventsTest : public ::testing::Test {
protected:
    void SetUp() override { Events::Clear(); }
    void TearDown() override { Events::Clear(); }
};

TEST_F(EventsTest, DispatchCallsRegisteredHandler) {
    int count = 0;
    Events::Register(Type::Render, [&] { count++; });
    Events::Dispatch(Type::Render);
    EXPECT_EQ(1, count);
}

TEST_F(EventsTest, DispatchCallsEveryHandlerForTheEvent) {
    int a = 0, b = 0;
    Events::Register(Type::Render, [&] { a++; });
    Events::Register(Type::Render, [&] { b++; });
    Events::Dispatch(Type::Render);
    EXPECT_EQ(1, a);
    EXPECT_EQ(1, b);
}

TEST_F(EventsTest, HandlersAreScopedToTheirEvent) {
    int render = 0, shutdown = 0;
    Events::Register(Type::Render, [&] { render++; });
    Events::Register(Type::Shutdown, [&] { shutdown++; });
    Events::Dispatch(Type::Render);
    EXPECT_EQ(1, render);
    EXPECT_EQ(0, shutdown);
}

TEST_F(EventsTest, DispatchEventWithNoHandlersIsNoop) {
    EXPECT_NO_THROW(Events::Dispatch(Type::LoadIntoMap));
}

TEST_F(EventsTest, HasHandlersAndEmpty) {
    EXPECT_TRUE(Events::Empty());
    EXPECT_FALSE(Events::HasHandlers(Type::Render));

    Events::Register(Type::Render, [] {});
    EXPECT_FALSE(Events::Empty());
    EXPECT_TRUE(Events::HasHandlers(Type::Render));
    EXPECT_FALSE(Events::HasHandlers(Type::Shutdown));
}

TEST_F(EventsTest, ThrowingHandlerIsContainedAndOthersStillRun) {
    int after = 0;
    Events::Register(Type::Render, [] { throw std::runtime_error("boom"); });
    Events::Register(Type::Render, [&] { after++; });
    EXPECT_NO_THROW(Events::Dispatch(Type::Render));
    EXPECT_EQ(1, after);
}

TEST_F(EventsTest, ClearRemovesHandlers) {
    Events::Register(Type::Render, [] {});
    Events::Clear();
    EXPECT_TRUE(Events::Empty());
}

}  // namespace
