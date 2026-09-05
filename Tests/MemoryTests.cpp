/**
 * @file
 * @brief Unit tests for our Memory signature-scanning utility (Internal/memory/Memory.h).
 *
 * Exercises the pure core against crafted buffers: IDA-string parsing, the 0x00-wildcard byte
 * convention, pattern search, RIP-relative resolution, and the compose that reproduces the old
 * FindPointer (scan a wildcard signature, then follow the displacement at the first wildcard).
 */

#include <cstdint>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

// Tests' IncludePath doesn't cover Internal/memory, so reach the header by relative path.
#include "../Internal/memory/Memory.h"

TEST(MemoryTest, ParseBuildsBytesAndMask)
{
	const Memory::Signature sig = Memory::Parse("48 8B ? ? 90");
	ASSERT_EQ(sig.size(), 5u);
	EXPECT_EQ(sig.bytes[0], 0x48);
	EXPECT_EQ(sig.bytes[1], 0x8B);
	EXPECT_EQ(sig.bytes[4], 0x90);
	EXPECT_TRUE(sig.mask[0]);
	EXPECT_TRUE(sig.mask[1]);
	EXPECT_FALSE(sig.mask[2]);
	EXPECT_FALSE(sig.mask[3]);
	EXPECT_TRUE(sig.mask[4]);
}

TEST(MemoryTest, ParseAcceptsDoubleQuestionWildcards)
{
	const Memory::Signature sig = Memory::Parse("48 ?? 90");
	ASSERT_EQ(sig.size(), 3u);
	EXPECT_TRUE(sig.mask[0]);
	EXPECT_FALSE(sig.mask[1]);
	EXPECT_TRUE(sig.mask[2]);
}

TEST(MemoryTest, FindLocatesPatternWithWildcards)
{
	uint8_t buffer[] = {0x00, 0x11, 0xAA, 0xBB, 0xCC, 0xDD, 0x22};
	const Memory::Signature sig = Memory::Parse("AA BB ? DD");
	uint8_t* hit = Memory::Find(buffer, buffer + sizeof(buffer), sig);
	EXPECT_EQ(hit, buffer + 2);
}

TEST(MemoryTest, FindReturnsNullWhenAbsent)
{
	uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
	const Memory::Signature sig = Memory::Parse("AA BB CC");
	EXPECT_EQ(Memory::Find(buffer, buffer + sizeof(buffer), sig), nullptr);
}

TEST(MemoryTest, FromBytesTreatsZeroAsWildcard)
{
	const uint8_t raw[] = {0x11, 0x00, 0x33};
	const Memory::Signature sig = Memory::FromBytes(raw, sizeof(raw));
	ASSERT_EQ(sig.size(), 3u);
	EXPECT_TRUE(sig.mask[0]);
	EXPECT_FALSE(sig.mask[1]); // the 0x00 became a wildcard
	EXPECT_TRUE(sig.mask[2]);

	uint8_t match[] = {0x11, 0x7E, 0x33};  // middle byte is free
	uint8_t nomatch[] = {0x11, 0x7E, 0x34}; // last byte differs
	EXPECT_TRUE(Memory::Matches(match, sig));
	EXPECT_FALSE(Memory::Matches(nomatch, sig));
}

TEST(MemoryTest, RelativeFollowsDisplacement)
{
	// "xx xx xx <int32 disp = 0x10>" — Relative reads the disp at offset 3 and returns
	// base + 3 + 4 + disp.
	uint8_t buffer[16] = {0x48, 0x8B, 0x05, 0x10, 0x00, 0x00, 0x00};
	uint8_t* target = Memory::Relative(buffer, 3);
	EXPECT_EQ(target, buffer + 3 + 4 + 0x10);
}

TEST(MemoryTest, RelativeHandlesNegativeDisplacement)
{
	uint8_t buffer[16] = {0};
	const int32_t disp = -4;
	std::memcpy(buffer + 3, &disp, sizeof(disp));
	uint8_t* target = Memory::Relative(buffer, 3);
	EXPECT_EQ(target, buffer + 3 + 4 - 4);
}

TEST(MemoryTest, FindStringMatchesNullTerminated)
{
	uint8_t buf[] = {0x00, 'h', 'i', 0x00, 'a', 'b', 'c', 0x00};
	EXPECT_EQ(Memory::FindString(buf, buf + sizeof(buf), "abc"), buf + 4);
	EXPECT_EQ(Memory::FindString(buf, buf + sizeof(buf), "hi"), buf + 1);
	EXPECT_EQ(Memory::FindString(buf, buf + sizeof(buf), "zz"), nullptr);
	// "ab" must NOT match inside "abc" (the trailing NUL guard).
	EXPECT_EQ(Memory::FindString(buf, buf + sizeof(buf), "ab"), nullptr);
}

TEST(MemoryTest, FindLeaToResolvesRipRelative)
{
	uint8_t buf[32] = {0};
	buf[0] = 0x48; // REX.W
	buf[1] = 0x8D; // lea
	buf[2] = 0x05; // mod=00, reg=000, rm=101 (rip-relative)
	const int32_t disp = 16 - 7; // target = at+7+disp = 16
	std::memcpy(buf + 3, &disp, sizeof(disp));

	EXPECT_EQ(Memory::FindLeaTo(buf, buf + sizeof(buf), buf + 16), buf);
	EXPECT_EQ(Memory::FindLeaTo(buf, buf + sizeof(buf), buf + 20), nullptr);
}

TEST(MemoryTest, FindThenRelativeReproducesPointerLookup)
{
	// The FindPointer path: a "mov reg, [rip+disp]" (48 8B 05 ?? ?? ?? ??) somewhere in a buffer.
	// Scan with a 0x00-wildcard signature, take the displacement at the first wildcard, resolve it.
	uint8_t buffer[32] = {0};
	buffer[8] = 0x48;
	buffer[9] = 0x8B;
	buffer[10] = 0x05;
	const int32_t disp = 3;
	std::memcpy(buffer + 11, &disp, sizeof(disp)); // disp bytes at 11..14

	const uint8_t rawSig[] = {0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00};
	const Memory::Signature sig = Memory::FromBytes(rawSig, sizeof(rawSig));
	uint8_t* at = Memory::Find(buffer, buffer + sizeof(buffer), sig);
	ASSERT_EQ(at, buffer + 8);

	int dispOffset = 0;
	while (dispOffset < static_cast<int>(sizeof(rawSig)) && sig.mask[dispOffset]) dispOffset++;
	EXPECT_EQ(dispOffset, 3);

	uint8_t* resolved = Memory::Relative(at, dispOffset);
	EXPECT_EQ(resolved, at + 3 + 4 + disp);
}
