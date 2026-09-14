#pragma once

#include "Fwd.h"
#include "TArray.h"

// NOTE: this FString owns its buffer. Every constructor copies into freshly-allocated storage and
// the destructor frees it (copy/move keep exactly one owner), so the short strings the ESP builds
// each frame — distances, names — no longer leak. The layout stays {Data, Count, Max} (via TArray)
// so it is still ABI-compatible when passed to ProcessEvent. FStrings living in game memory are only
// ever read (and deep-copied when passed by value), never freed by us.
struct FString : private TArray<wchar_t>
{
	inline FString() {
	};

	FString(const wchar_t* other)
	{
		assign(other, other ? static_cast<int32_t>(std::wcslen(other)) : 0);
	};

	FString(const std::string& other)
	{
		if (other.empty()) return;

		Count = Max = static_cast<int32_t>(other.length()) + 1;
		Data = new wchar_t[Count];
		std::copy(other.begin(), other.end(), Data);
		Data[Count - 1] = L'\0';
	}

	FString(const FString& other) { assign(other.Data, other.Count > 0 ? other.Count - 1 : 0); }

	FString(FString&& other) noexcept
	{
		Data = other.Data;
		Count = other.Count;
		Max = other.Max;
		other.Data = nullptr;
		other.Count = other.Max = 0;
	}

	FString& operator=(const FString& other)
	{
		if (this != &other)
		{
			release();
			assign(other.Data, other.Count > 0 ? other.Count - 1 : 0);
		}
		return *this;
	}

	FString& operator=(FString&& other) noexcept
	{
		if (this != &other)
		{
			release();
			Data = other.Data;
			Count = other.Count;
			Max = other.Max;
			other.Data = nullptr;
			other.Count = other.Max = 0;
		}
		return *this;
	}

	~FString() { release(); }

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		if (!Data) return {};

		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}

  private:
	/// Copy @p len wide chars (+ null terminator) from @p src into freshly-allocated owned storage.
	void assign(const wchar_t* src, int32_t len)
	{
		if (!src || len <= 0)
		{
			Data = nullptr;
			Count = Max = 0;
			return;
		}

		Count = Max = len + 1;
		Data = new wchar_t[Count];
		std::copy(src, src + len, Data);
		Data[len] = L'\0';
	}

	/// Free the owned buffer.
	void release()
	{
		delete[] Data;
		Data = nullptr;
		Count = Max = 0;
	}
};
