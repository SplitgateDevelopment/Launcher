// Unit tests for the feature framework (Feature base class + Features::Execute).
//
// The concrete, game-specific features live behind the UE SDK and cannot be
// exercised off the game; instead we register fake features against the same
// Feature interface and drive them through Features::Execute().
#define NOMINMAX

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "FeatureRunner.h"

namespace {

// A Feature stand-in that records how the framework drove it.
struct FakeFeature : public Feature {
    int initCount = 0;
    int checkCount = 0;
    int runCount = 0;
    int destroyCount = 0;
    bool checkResult = true;
    bool throwOnRun = false;
    bool legacyCheck = false;  // emulate the old contract where Check() returns Enabled

    explicit FakeFeature(std::string name = "Fake") { Name = name; }

    void Init() override { initCount++; Initialized = true; }
    void UpdateEnabled() override {}
    bool Check() override { checkCount++; return legacyCheck ? Enabled : checkResult; }
    void Destroy() override { destroyCount++; }
    void Run() override { runCount++; if (throwOnRun) throw std::runtime_error("boom"); }
};

FakeFeature* add(std::string name = "Fake") {
    auto feature = std::make_unique<FakeFeature>(name);
    FakeFeature* raw = feature.get();
    Features::Features.push_back(std::move(feature));
    return raw;
}

class FeaturesTest : public ::testing::Test {
protected:
    void SetUp() override { Features::Features.clear(); }
    void TearDown() override { Features::Features.clear(); }
};

TEST_F(FeaturesTest, InitializesUninitializedFeatures) {
    FakeFeature* f = add();
    ASSERT_FALSE(f->Initialized);
    Features::Execute();
    EXPECT_EQ(1, f->initCount);
    EXPECT_TRUE(f->Initialized);
}

TEST_F(FeaturesTest, DoesNotReinitializeInitializedFeatures) {
    FakeFeature* f = add();
    Features::Execute();
    Features::Execute();
    EXPECT_EQ(1, f->initCount);
}

TEST_F(FeaturesTest, SkipsRunAndDestroyWhenCheckFails) {
    FakeFeature* f = add();
    f->checkResult = false;
    f->Enabled = true;
    Features::Execute();
    EXPECT_EQ(1, f->checkCount);
    EXPECT_EQ(0, f->runCount);
    EXPECT_EQ(0, f->destroyCount);
}

TEST_F(FeaturesTest, RunsWhenEnabledAndCheckPasses) {
    FakeFeature* f = add();
    f->Enabled = true;
    Features::Execute();
    EXPECT_EQ(1, f->runCount);
    EXPECT_EQ(0, f->destroyCount);
}

TEST_F(FeaturesTest, DoesNotDestroyWhenNeverApplied) {
    FakeFeature* f = add();
    f->Enabled = false;  // disabled from the start, so nothing was ever applied
    Features::Execute();
    Features::Execute();
    EXPECT_EQ(0, f->runCount);
    EXPECT_EQ(0, f->destroyCount);
}

TEST_F(FeaturesTest, SkipsValidityCheckForIdleDisabledFeatures) {
    FakeFeature* f = add();
    f->Enabled = false;              // disabled, nothing applied
    Features::Execute();
    Features::Execute();
    EXPECT_EQ(0, f->checkCount);     // Check() (the validity walk) is skipped
    EXPECT_EQ(1, f->initCount);      // but it is still initialized once
}

TEST_F(FeaturesTest, DestroysOnceOnDisableTransition) {
    FakeFeature* f = add();

    f->Enabled = true;
    Features::Execute();           // applied
    EXPECT_EQ(1, f->runCount);

    f->Enabled = false;
    Features::Execute();           // enabled -> disabled edge: revert once
    Features::Execute();           // still disabled: no repeated Destroy
    Features::Execute();
    EXPECT_EQ(1, f->destroyCount);
}

TEST_F(FeaturesTest, OneTimeRunsOncePerEnable) {
    FakeFeature* f = add();
    f->OneTime = true;
    f->Enabled = true;
    Features::Execute();
    Features::Execute();
    Features::Execute();
    EXPECT_EQ(1, f->runCount);
}

TEST_F(FeaturesTest, OneTimeReArmsAfterDisable) {
    FakeFeature* f = add();
    f->OneTime = true;

    f->Enabled = true;
    Features::Execute();           // run #1
    f->Enabled = false;
    Features::Execute();           // disable re-arms
    f->Enabled = true;
    Features::Execute();           // run #2
    EXPECT_EQ(2, f->runCount);
}

// A legacy feature whose Check() returns Enabled keeps its old behavior: when
// disabled, Check() returns false, the loop skips it, and Destroy() is never
// reached. This guards the migration path for features still on the old contract.
TEST_F(FeaturesTest, LegacyCheckReturningEnabledNeverDestroys) {
    FakeFeature* f = add();
    f->legacyCheck = true;

    f->Enabled = true;
    Features::Execute();           // Check() true, Run
    f->Enabled = false;
    Features::Execute();           // Check() returns Enabled == false -> skipped
    Features::Execute();
    EXPECT_EQ(1, f->runCount);
    EXPECT_EQ(0, f->destroyCount);
}

TEST_F(FeaturesTest, ProcessesEveryRegisteredFeature) {
    FakeFeature* a = add("A");
    FakeFeature* b = add("B");
    a->Enabled = true;
    b->Enabled = true;
    Features::Execute();
    EXPECT_EQ(1, a->runCount);
    EXPECT_EQ(1, b->runCount);
}

// Execute only guards against a thrown char* (the codebase's error convention);
// its logging path also requires the game's Logger, which a standalone test
// process can't initialize. So we document the boundary instead: any other
// exception type propagates out of Execute. If the catch is ever broadened,
// this test is the deliberate place to revisit.
TEST_F(FeaturesTest, PropagatesNonCharPointerExceptions) {
    FakeFeature* f = add();
    f->Enabled = true;
    f->throwOnRun = true;
    EXPECT_THROW(Features::Execute(), std::runtime_error);
}

}  // namespace
