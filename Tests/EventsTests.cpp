// Unit tests for the Python-free event registry (scripting/Events.h). The
// pybind11 bridge (modules/Events.h) and the game-side dispatch (ProcessEvent)
// need the game and are not unit tested; the registry logic is.
#include <gtest/gtest.h>

#include <stdexcept>

#include "Events.h"

namespace {

class EventsTest : public ::testing::Test {
protected:
    void SetUp() override { Events::Clear(); }
    void TearDown() override { Events::Clear(); }
};

TEST_F(EventsTest, DispatchCallsRegisteredHandler) {
    int count = 0;
    Events::Register("e", [&] { count++; });
    Events::Dispatch("e");
    EXPECT_EQ(1, count);
}

TEST_F(EventsTest, DispatchCallsEveryHandlerForTheEvent) {
    int a = 0, b = 0;
    Events::Register("e", [&] { a++; });
    Events::Register("e", [&] { b++; });
    Events::Dispatch("e");
    EXPECT_EQ(1, a);
    EXPECT_EQ(1, b);
}

TEST_F(EventsTest, HandlersAreScopedToTheirEvent) {
    int a = 0, b = 0;
    Events::Register("a", [&] { a++; });
    Events::Register("b", [&] { b++; });
    Events::Dispatch("a");
    EXPECT_EQ(1, a);
    EXPECT_EQ(0, b);
}

TEST_F(EventsTest, DispatchUnknownEventIsNoop) {
    EXPECT_NO_THROW(Events::Dispatch("nothing-here"));
}

TEST_F(EventsTest, HasHandlersAndEmpty) {
    EXPECT_TRUE(Events::Empty());
    EXPECT_FALSE(Events::HasHandlers("e"));

    Events::Register("e", [] {});
    EXPECT_FALSE(Events::Empty());
    EXPECT_TRUE(Events::HasHandlers("e"));
    EXPECT_FALSE(Events::HasHandlers("other"));
}

TEST_F(EventsTest, ThrowingHandlerIsContainedAndOthersStillRun) {
    int after = 0;
    Events::Register("e", [] { throw std::runtime_error("boom"); });
    Events::Register("e", [&] { after++; });
    EXPECT_NO_THROW(Events::Dispatch("e"));
    EXPECT_EQ(1, after);
}

TEST_F(EventsTest, ClearRemovesHandlers) {
    Events::Register("e", [] {});
    Events::Clear();
    EXPECT_TRUE(Events::Empty());
}

}  // namespace
