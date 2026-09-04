/// @file
/// @brief Unit tests for the pure core of the memcury-style AOB engine (Internal/utils/Memcury.h):
/// pattern parsing, wildcard scanning, RIP-relative resolution, and string-reference discovery.
/// These run against crafted byte buffers, so no game/module is needed.

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

#include "../Internal/utils/Memcury.h"

namespace
{
	TEST(MemcuryTest, ParsesIdaPatternWithWildcards)
	{
		const auto pattern = Memcury::Parse("48 8B ?? E8 ?? ?? ?? ??");
		ASSERT_EQ(8u, pattern.size());
		EXPECT_EQ(0x48, pattern[0].value);
		EXPECT_FALSE(pattern[0].wildcard);
		EXPECT_EQ(0x8B, pattern[1].value);
		EXPECT_TRUE(pattern[2].wildcard);
		EXPECT_EQ(0xE8, pattern[3].value);
		EXPECT_TRUE(pattern[7].wildcard);
	}

	TEST(MemcuryTest, FindPatternMatchesWithWildcards)
	{
		uint8_t buf[] = {0x00, 0x11, 0x48, 0x8B, 0xCC, 0xE8, 0x01, 0x02, 0x03, 0x04, 0x90};
		uint8_t* hit = Memcury::FindPattern("48 8B ?? E8", buf, sizeof(buf));
		ASSERT_NE(nullptr, hit);
		EXPECT_EQ(buf + 2, hit);
	}

	TEST(MemcuryTest, FindPatternReturnsNullWhenAbsent)
	{
		uint8_t buf[] = {0x00, 0x11, 0x22, 0x33};
		EXPECT_EQ(nullptr, Memcury::FindPattern("DE AD BE EF", buf, sizeof(buf)));
	}

	TEST(MemcuryTest, RelativeOffsetFollowsANearCall)
	{
		// A near call at buf+10: E8 <disp32>, disp chosen so the target is buf+20.
		// disp = target - (call + 5) = (buf+20) - (buf+15) = 5.
		std::vector<uint8_t> buf(40, 0);
		buf[10] = 0xE8;
		const int32_t disp = 5;
		std::memcpy(&buf[11], &disp, sizeof(disp));

		Memcury::Scanner scanner(buf.data() + 10);
		EXPECT_EQ(buf.data() + 20, scanner.RelativeOffset(1).Get());
	}

	TEST(MemcuryTest, FindStringRefLocatesTheLea)
	{
		std::vector<uint8_t> buf(80, 0);

		// "hello\0" at buf+50.
		const char* str = "hello";
		std::memcpy(&buf[50], str, 6);

		// lea rax, [rip+disp] at buf+30: 48 8D 05 <disp32>.
		// disp = strAddr - (lea + 7) = (buf+50) - (buf+37) = 13.
		buf[30] = 0x48;
		buf[31] = 0x8D;
		buf[32] = 0x05;
		const int32_t disp = 13;
		std::memcpy(&buf[33], &disp, sizeof(disp));

		uint8_t* ref = Memcury::FindStringRef("hello", buf.data(), buf.size());
		EXPECT_EQ(buf.data() + 30, ref);
	}

	TEST(MemcuryTest, FindStringRefNullWhenStringMissing)
	{
		std::vector<uint8_t> buf(16, 0);
		EXPECT_EQ(nullptr, Memcury::FindStringRef("nope", buf.data(), buf.size()));
	}
} // namespace
