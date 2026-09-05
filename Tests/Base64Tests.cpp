/// @file
/// @brief Unit tests for the base64 helper (Internal/utils/Base64.h) used by settings share codes.
///
/// Covers the RFC 4648 test vectors, encode/decode round-trips over binary data (all byte values),
/// the three padding lengths, and the documented decode behaviour of stopping at the first
/// non-base64 character.

#include <string>

#include <gtest/gtest.h>

// Tests' IncludePath doesn't cover Internal/utils, so reach the header by relative path.
#include "../Internal/utils/Base64.h"

TEST(Base64Test, EncodesKnownVectors)
{
	// RFC 4648 §10 examples; exercise all three padding cases.
	EXPECT_EQ(Base64::Encode(""), "");
	EXPECT_EQ(Base64::Encode("f"), "Zg==");
	EXPECT_EQ(Base64::Encode("fo"), "Zm8=");
	EXPECT_EQ(Base64::Encode("foo"), "Zm9v");
	EXPECT_EQ(Base64::Encode("foob"), "Zm9vYg==");
	EXPECT_EQ(Base64::Encode("fooba"), "Zm9vYmE=");
	EXPECT_EQ(Base64::Encode("foobar"), "Zm9vYmFy");

	EXPECT_EQ(Base64::Encode("M"), "TQ==");
	EXPECT_EQ(Base64::Encode("Ma"), "TWE=");
	EXPECT_EQ(Base64::Encode("Man"), "TWFu");
}

TEST(Base64Test, DecodesKnownVectors)
{
	EXPECT_EQ(Base64::Decode(""), "");
	EXPECT_EQ(Base64::Decode("Zg=="), "f");
	EXPECT_EQ(Base64::Decode("Zm8="), "fo");
	EXPECT_EQ(Base64::Decode("Zm9v"), "foo");
	EXPECT_EQ(Base64::Decode("Zm9vYg=="), "foob");
	EXPECT_EQ(Base64::Decode("Zm9vYmE="), "fooba");
	EXPECT_EQ(Base64::Decode("Zm9vYmFy"), "foobar");
}

TEST(Base64Test, OutputLengthIsPaddedToMultipleOfFour)
{
	// One, two and three trailing bytes each still yield a length divisible by four.
	EXPECT_EQ(Base64::Encode("M").size() % 4, 0u);
	EXPECT_EQ(Base64::Encode("Ma").size() % 4, 0u);
	EXPECT_EQ(Base64::Encode("Man").size() % 4, 0u);
}

TEST(Base64Test, RoundTripsAllByteValues)
{
	// A payload containing every byte 0x00..0xFF must survive encode -> decode unchanged.
	std::string raw;
	raw.reserve(256);
	for (int b = 0; b < 256; ++b)
		raw.push_back(static_cast<char>(b));

	EXPECT_EQ(Base64::Decode(Base64::Encode(raw)), raw);
}

TEST(Base64Test, RoundTripsEveryPayloadLengthShift)
{
	// Cover all three "bits" phases of the encoder across successive lengths.
	std::string raw;
	for (int i = 0; i < 20; ++i)
	{
		raw.push_back(static_cast<char>('a' + i));
		EXPECT_EQ(Base64::Decode(Base64::Encode(raw)), raw);
	}
}

TEST(Base64Test, DecodeStopsAtFirstNonBase64Char)
{
	// Documented behaviour: decode halts at the first character outside the alphabet (padding
	// included), so trailing junk after a complete group is ignored.
	EXPECT_EQ(Base64::Decode("Zm9v Zm9v"), "foo"); // space ends decoding after the first group
	EXPECT_EQ(Base64::Decode("Zm9v!!!!"), "foo");
	EXPECT_EQ(Base64::Decode("Zm9v========"), "foo"); // padding is a non-alphabet char, so it stops
}
