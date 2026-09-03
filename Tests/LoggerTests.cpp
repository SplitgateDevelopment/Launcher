// Unit tests for the shared console logger (shared/Logger.h). The console/color side needs a
// real console, so these cover the file-mirroring behavior: level tagging and the timestamp
// prefix. The logger is scoped so its destructor closes the file before it is read back.
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "Logger.h"

namespace {

namespace fs = std::filesystem;

std::string ReadAll(const fs::path& path) {
	std::ifstream in(path);
	std::stringstream ss;
	ss << in.rdbuf();
	return ss.str();
}

class LoggerTest : public ::testing::Test {
protected:
	fs::path logPath;

	void SetUp() override {
		logPath = fs::temp_directory_path() / "sg_logger_test.log";
		std::error_code ec;
		fs::remove(logPath, ec);
	}

	void TearDown() override {
		std::error_code ec;
		fs::remove(logPath, ec);
	}
};

TEST_F(LoggerTest, MirrorsLinesToFileWithLevel) {
	{
		Shared::Logger logger;
		logger.attachConsole(logPath.string());
		logger.info("hello world");
		logger.error("boom");
	}

	const std::string contents = ReadAll(logPath);
	EXPECT_NE(contents.find("[INFO] hello world"), std::string::npos);
	EXPECT_NE(contents.find("[ERROR] boom"), std::string::npos);
}

TEST_F(LoggerTest, PrefixesEachLineWithATimestamp) {
	{
		Shared::Logger logger;
		logger.attachConsole(logPath.string());
		logger.info("x");
	}

	const std::string contents = ReadAll(logPath);
	ASSERT_FALSE(contents.empty());
	EXPECT_EQ(contents.front(), '['); // [HH:MM:SS] ...
	EXPECT_NE(contents.find("] [INFO] x"), std::string::npos);
}

TEST(LoggerNoFileTest, LoggingWithoutAFileIsSafe) {
	Shared::Logger logger; // no attachConsole → no log file
	EXPECT_NO_THROW(logger.info("no file"));
}

} // namespace
