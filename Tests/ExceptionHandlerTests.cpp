/// @file
/// @brief Unit tests for the shared crash handler (shared/ExceptionHandler.h) via WriteCrashLog.

// Unit tests for the shared crash handler (shared/ExceptionHandler.h). We can't unit test an
// actual unhandled exception (it would terminate the runner), so we drive the testable core:
// WriteCrashLog with a real context from RtlCaptureContext, pointed at a temp folder.
#include <gtest/gtest.h>

#include <Windows.h>

#include <filesystem>
#include <string>

#include "ExceptionHandler.h"

namespace
{

	namespace fs = std::filesystem;
	namespace EH = Shared::ExceptionHandler;

	/// Fixture providing a clean temp crash directory, removed before and after each test.
	class ExceptionHandlerTest : public ::testing::Test
	{
	  protected:
		fs::path crashDir;

		void SetUp() override
		{
			crashDir = fs::temp_directory_path() / "sg_eh_tests";
			std::error_code ec;
			fs::remove_all(crashDir, ec);
		}

		void TearDown() override
		{
			std::error_code ec;
			fs::remove_all(crashDir, ec);
		}
	};

	TEST_F(ExceptionHandlerTest, TimestampHasExpectedShape)
	{
		const std::string ts = EH::CrashTimestamp(); // YYYY-MM-DD-HH-MM-SS
		EXPECT_EQ(ts.size(), 19u);
		EXPECT_EQ(ts[4], '-');
		EXPECT_EQ(ts[7], '-');
		EXPECT_EQ(ts[10], '-');
	}

	TEST_F(ExceptionHandlerTest, WritesReportAndRunsRecovery)
	{
		bool recovered = false;
		int logCount = 0;

		EH::Config config;
		config.crashDir = crashDir;
		config.exitMode = EH::ExitMode::Silent;
		config.log = [&](const std::string&, const std::string&)
		{ ++logCount; };
		config.onCrash = [&]()
		{ recovered = true; };

		CONTEXT context{};
		RtlCaptureContext(&context);
		const LONG rc = EH::WriteCrashLog(config, 0xC0000005, &context);

		EXPECT_EQ(rc, static_cast<LONG>(EH::ExitMode::Silent));
		EXPECT_TRUE(recovered);
		EXPECT_GE(logCount, 2);

		bool wroteReport = false;
		for (const auto& entry : fs::recursive_directory_iterator(crashDir))
			if (entry.path().filename() == L"StackTrace.log") wroteReport = true;
		EXPECT_TRUE(wroteReport);
	}

	TEST_F(ExceptionHandlerTest, RunsWithoutCallbacksSet)
	{
		EH::Config config;
		config.crashDir = crashDir; // no log / no onCrash

		CONTEXT context{};
		RtlCaptureContext(&context);
		EXPECT_NO_THROW(EH::WriteCrashLog(config, 0x1, &context));
	}

	TEST(ExceptionHandlerInstallTest, InstallAndUninstallDoNotThrow)
	{
		EH::Config config;
		config.crashDir = fs::temp_directory_path() / "sg_eh_install";
		EXPECT_NO_THROW(EH::Install(config));
		EXPECT_NO_THROW(EH::Uninstall());
	}

} // namespace
