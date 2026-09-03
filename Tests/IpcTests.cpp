// Unit tests for the launcher/DLL IPC signaling helper (shared/Ipc.h). The named event is
// created, signaled and waited on within this single test process — no game required.
#include <gtest/gtest.h>

#include <Windows.h>

#include "Ipc.h"

namespace {

TEST(IpcTest, NameIsStableAndSessionLocal) {
	EXPECT_STREQ(Ipc::Name(Ipc::Event::Initialized), L"Local\\SplitgateInitialized");
}

TEST(IpcTest, SignalWakesAWaiter) {
	HANDLE event = Ipc::Create(Ipc::Event::Initialized);
	ASSERT_NE(event, nullptr);

	EXPECT_FALSE(Ipc::Wait(event, 0)); // not signaled yet
	EXPECT_TRUE(Ipc::Signal(Ipc::Event::Initialized));
	EXPECT_TRUE(Ipc::Wait(event, 0)); // manual-reset stays signaled

	CloseHandle(event);
}

TEST(IpcTest, SignalWithoutListenerReturnsFalse) {
	// No event exists (nothing created it) → OpenEventW fails → Signal reports false.
	EXPECT_FALSE(Ipc::Signal(Ipc::Event::Initialized));
}

} // namespace
