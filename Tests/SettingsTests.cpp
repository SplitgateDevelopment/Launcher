/// @file
/// @brief Unit tests for Internal/settings — save/load/serialization exercised in isolation.

// Unit tests for Internal/settings — scoped to save/load/serialization.
//
// Settings.cpp is compiled directly into this test target (see Tests.vcxproj);
// it has no dependency on the game, hooks, UE SDK or D3D11, so it can be
// exercised in isolation.
#define NOMINMAX // keep <Windows.h> (pulled in by Settings.h) from clobbering std::min/max

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "Settings.h"

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// Pure serialization: no filesystem, fast and deterministic. Exercises the
// nlohmann round-trip that Save()/Load() rely on.
// ---------------------------------------------------------------------------
TEST(SettingsSerialization, JsonRoundTripPreservesPersistedFields)
{
	SETTINGS in{};
	in.EXPLOITS.FOV = 95.f;
	in.EXPLOITS.NoRecoil = true;
	in.EXPLOITS.PlayerSpeed = 1.75f;
	in.MISC.PlayerName = "roundtrip";

	const json j = in;
	const auto out = j.get<SETTINGS>();

	EXPECT_FLOAT_EQ(95.f, out.EXPLOITS.FOV);
	EXPECT_TRUE(out.EXPLOITS.NoRecoil);
	EXPECT_FLOAT_EQ(1.75f, out.EXPLOITS.PlayerSpeed);
	EXPECT_EQ("roundtrip", out.MISC.PlayerName);
}

// ---------------------------------------------------------------------------
// Enum <-> JSON mappings (NLOHMANN_JSON_SERIALIZE_ENUM). These guard the string
// names the settings file persists against accidental enum/mapping drift, and
// pin the documented fallback: an unknown string decodes to the first mapped
// entry.
// ---------------------------------------------------------------------------
TEST(SettingsSerialization, RendererModeMapsToNames)
{
	EXPECT_EQ(json(RendererMode::Canvas).get<std::string>(), "canvas");
	EXPECT_EQ(json(RendererMode::ImGui).get<std::string>(), "imgui");
	EXPECT_EQ(json(RendererMode::Null).get<std::string>(), "null");

	EXPECT_EQ(json("canvas").get<RendererMode>(), RendererMode::Canvas);
	EXPECT_EQ(json("imgui").get<RendererMode>(), RendererMode::ImGui);
	EXPECT_EQ(json("null").get<RendererMode>(), RendererMode::Null);
}

TEST(SettingsSerialization, RendererModeUnknownFallsBackToFirst)
{
	// nlohmann's SERIALIZE_ENUM returns the first listed pair for an unmapped value.
	EXPECT_EQ(json("bogus").get<RendererMode>(), RendererMode::Canvas);
}

TEST(SettingsSerialization, CameraModeMapsToNames)
{
	EXPECT_EQ(json(CameraMode::FirstPerson).get<std::string>(), "first");
	EXPECT_EQ(json(CameraMode::ThirdPerson).get<std::string>(), "third");
	EXPECT_EQ(json(CameraMode::FreeCam).get<std::string>(), "free");

	EXPECT_EQ(json("first").get<CameraMode>(), CameraMode::FirstPerson);
	EXPECT_EQ(json("third").get<CameraMode>(), CameraMode::ThirdPerson);
	EXPECT_EQ(json("free").get<CameraMode>(), CameraMode::FreeCam);
}

TEST(SettingsSerialization, CameraModeUnknownFallsBackToFirst)
{
	EXPECT_EQ(json("bogus").get<CameraMode>(), CameraMode::FirstPerson);
}

TEST(SettingsSerialization, RendererAndCameraSurviveSettingsRoundTrip)
{
	SETTINGS in{};
	in.MENU.Renderer = RendererMode::ImGui;
	in.EXPLOITS.Camera = CameraMode::ThirdPerson;

	const auto out = json(in).get<SETTINGS>();

	EXPECT_EQ(out.MENU.Renderer, RendererMode::ImGui);
	EXPECT_EQ(out.EXPLOITS.Camera, CameraMode::ThirdPerson);
}

// ---------------------------------------------------------------------------
// Save/Load hit the real Documents\SplitgateInternal\splitgate.settings path
// (the API exposes no injectable location). This fixture moves any real user
// settings aside for the duration of each test and restores them afterwards,
// and resets the in-memory Settings to defaults around each test so cases stay
// independent of one another.
// ---------------------------------------------------------------------------
/// Fixture that isolates disk-backed settings: backs up any real file, resets to defaults, restores after.
class SettingsFileTest : public ::testing::Test
{
  protected:
	std::string path;
	fs::path backup;
	bool hadBackup = false;

	void SetUp() override
	{
		path = SettingsHelper::File().Path().string();
		backup = fs::path(path).concat(".uttmp");

		std::error_code ec;
		if (fs::exists(path))
		{
			fs::rename(path, backup, ec);
			hadBackup = !ec;
		}
		SettingsHelper::File().Reset();
	}

	void TearDown() override
	{
		std::error_code ec;
		fs::remove(path, ec);
		if (hadBackup) fs::rename(backup, path, ec);
		SettingsHelper::File().Reset();
	}
};

TEST_F(SettingsFileTest, SaveCreatesFile)
{
	ASSERT_FALSE(fs::exists(path));
	SettingsHelper::File().Save();
	EXPECT_TRUE(fs::exists(path));
}

TEST_F(SettingsFileTest, SaveThenLoadRoundTripsPersistedFields)
{
	// Move every persisted field off its default.
	Settings.MENU.ShowMenu = false;
	Settings.MENU.ShowWatermark = false;
	Settings.MENU.ShowHotkey = VK_END;
	Settings.MENU.Rgb = true;

	Settings.EXPLOITS.FOV = 120.f;
	Settings.EXPLOITS.GodMode = true;
	Settings.EXPLOITS.SpinBot = true;
	Settings.EXPLOITS.NoClip = VK_OEM_MINUS;
	Settings.EXPLOITS.NoRecoil = true;
	Settings.EXPLOITS.GodMelee = true;
	Settings.EXPLOITS.PlayerSpeed = 2.5f;
	Settings.EXPLOITS.InfinteJetpack = true;
	Settings.EXPLOITS.InfiniteAmmo = true;
	Settings.EXPLOITS.NoReload = true;

	Settings.MISC.PlayerName = "tester";
	Settings.MISC.UserScriptsEnabled = true;
	Settings.MISC.DiscordRPCEnabled = false;

	Settings.DEBUG.LogProcessEvent = true;
	Settings.DEBUG.FeaturesLogging = true;
	Settings.DEBUG.ShowDemoWindow = true;
	Settings.DEBUG.ShowStyleEditor = true;

	SettingsHelper::File().Save();
	SettingsHelper::File().Reset();				// wipe in-memory state...
	ASSERT_TRUE(Settings.MENU.ShowMenu);		// ...confirm it's back to default
	ASSERT_TRUE(SettingsHelper::File().Load()); // ...then reload from disk

	EXPECT_FALSE(Settings.MENU.ShowMenu);
	EXPECT_FALSE(Settings.MENU.ShowWatermark);
	EXPECT_EQ(VK_END, Settings.MENU.ShowHotkey);
	EXPECT_TRUE(Settings.MENU.Rgb);

	EXPECT_FLOAT_EQ(120.f, Settings.EXPLOITS.FOV);
	EXPECT_TRUE(Settings.EXPLOITS.GodMode);
	EXPECT_TRUE(Settings.EXPLOITS.SpinBot);
	EXPECT_EQ(VK_OEM_MINUS, Settings.EXPLOITS.NoClip);
	EXPECT_TRUE(Settings.EXPLOITS.NoRecoil);
	EXPECT_TRUE(Settings.EXPLOITS.GodMelee);
	EXPECT_FLOAT_EQ(2.5f, Settings.EXPLOITS.PlayerSpeed);
	EXPECT_TRUE(Settings.EXPLOITS.InfinteJetpack);
	EXPECT_TRUE(Settings.EXPLOITS.InfiniteAmmo);
	EXPECT_TRUE(Settings.EXPLOITS.NoReload);

	EXPECT_EQ("tester", Settings.MISC.PlayerName);
	EXPECT_TRUE(Settings.MISC.UserScriptsEnabled);
	EXPECT_FALSE(Settings.MISC.DiscordRPCEnabled);

	EXPECT_TRUE(Settings.DEBUG.LogProcessEvent);
	EXPECT_TRUE(Settings.DEBUG.FeaturesLogging);
	EXPECT_TRUE(Settings.DEBUG.ShowDemoWindow);
	EXPECT_TRUE(Settings.DEBUG.ShowStyleEditor);
}

TEST_F(SettingsFileTest, LoadReturnsFalseWhenFileMissing)
{
	ASSERT_FALSE(fs::exists(path));
	EXPECT_FALSE(SettingsHelper::File().Load());
}

TEST_F(SettingsFileTest, LoadReturnsFalseOnCorruptFile)
{
	{
		std::ofstream(path) << "{ this is not valid json ";
	}
	ASSERT_TRUE(fs::exists(path));
	EXPECT_FALSE(SettingsHelper::File().Load()); // must not throw
}

// With the WITH_DEFAULT macros, a settings file that predates a field (missing
// key) still loads — absent fields fall back to their defaults instead of
// failing the whole load.
TEST_F(SettingsFileTest, LoadToleratesMissingKeys)
{
	{
		std::ofstream(path) << "{}";
	}
	ASSERT_TRUE(SettingsHelper::File().Load());
	EXPECT_TRUE(Settings.MENU.ShowMenu);						 // default
	EXPECT_FLOAT_EQ(80.f, Settings.EXPLOITS.FOV);				 // default
	EXPECT_EQ("SplitgateDevelopment", Settings.MISC.PlayerName); // default
}

TEST_F(SettingsFileTest, DeleteRemovesFile)
{
	SettingsHelper::File().Save();
	ASSERT_TRUE(fs::exists(path));
	SettingsHelper::File().Remove();
	EXPECT_FALSE(fs::exists(path));
}

TEST_F(SettingsFileTest, ResetRestoresDefaults)
{
	Settings.EXPLOITS.FOV = 42.f;
	Settings.MISC.PlayerName = "changed";
	SettingsHelper::File().Reset();
	EXPECT_FLOAT_EQ(80.f, Settings.EXPLOITS.FOV);
	EXPECT_EQ("SplitgateDevelopment", Settings.MISC.PlayerName);
}

// ShowConsole was declared but missing from the MiscSettings macro, so it never
// persisted. It is now included and must round-trip through save/load.
TEST_F(SettingsFileTest, ShowConsolePersists)
{
	ASSERT_TRUE(Settings.MISC.ShowConsole); // default
	Settings.MISC.ShowConsole = false;

	SettingsHelper::File().Save();
	SettingsHelper::File().Reset();
	ASSERT_TRUE(Settings.MISC.ShowConsole); // reset back to default
	ASSERT_TRUE(SettingsHelper::File().Load());

	EXPECT_FALSE(Settings.MISC.ShowConsole);
}

// These fields are deliberately not exposed through the settings file: they are
// runtime/constant values, so they are intentionally left out of the NLOHMANN
// macros. Editing the json must not be able to change them, and they never get
// written — they always keep their defaults across a save/load.
TEST_F(SettingsFileTest, RuntimeOnlyFieldsAreNotPersisted)
{
	Settings.MENU.Watermark = "custom-watermark";
	Settings.MISC.DiscordAppID = "111222333";
	Settings.MISC.SteamAppId = "999999";

	SettingsHelper::File().Save();
	SettingsHelper::File().Reset();
	ASSERT_TRUE(SettingsHelper::File().Load());

	EXPECT_EQ("github.com/SplitgateDevelopment/Launcher", Settings.MENU.Watermark);
	EXPECT_EQ("1078744504066117703", Settings.MISC.DiscordAppID);
	EXPECT_EQ("677620", Settings.MISC.SteamAppId);
}
