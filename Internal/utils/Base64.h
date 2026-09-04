#pragma once

/// @file
/// @brief Minimal base64 encode/decode, used for settings "share codes".

#include <string>
#include <array>

namespace Base64
{
	inline constexpr const char* Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	/// Encode raw bytes to a base64 string (padded with '=').
	inline std::string Encode(const std::string& in)
	{
		std::string out;
		int val = 0, bits = -6;
		for (unsigned char c : in)
		{
			val = (val << 8) + c;
			bits += 8;
			while (bits >= 0)
			{
				out.push_back(Chars[(val >> bits) & 0x3F]);
				bits -= 6;
			}
		}
		if (bits > -6) out.push_back(Chars[((val << 8) >> (bits + 8)) & 0x3F]);
		while (out.size() % 4)
			out.push_back('=');
		return out;
	}

	/// Decode a base64 string back to raw bytes; stops at the first non-base64 char (e.g. padding).
	inline std::string Decode(const std::string& in)
	{
		std::array<int, 256> lookup;
		lookup.fill(-1);
		for (int i = 0; i < 64; i++)
			lookup[static_cast<unsigned char>(Chars[i])] = i;

		std::string out;
		int val = 0, bits = -8;
		for (unsigned char c : in)
		{
			if (lookup[c] == -1) break;
			val = (val << 6) + lookup[c];
			bits += 6;
			if (bits >= 0)
			{
				out.push_back(static_cast<char>((val >> bits) & 0xFF));
				bits -= 8;
			}
		}
		return out;
	}
} // namespace Base64
