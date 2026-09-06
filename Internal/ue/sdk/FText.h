#pragma once

#include "Fwd.h"

struct FText
{
	FTextData* Data;
	uint8_t Pad[0x10];

	FText() : Data(nullptr), Pad{} {}

	FText(const std::wstring& str) : Data(new FTextData), Pad{}
	{
		Data->Name = new wchar_t[str.length() + 1];
		std::copy(str.begin(), str.end(), Data->Name);
		Data->Name[str.length()] = L'\0';
	}

	wchar_t* Get() const
	{
		if (Data)
			return Data->Name;

		return nullptr;
	}

	std::string ToString()
	{
		if (!Data) return "";

		std::wstring temp(Data->Name);
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), -1, nullptr, 0, nullptr, nullptr);

		std::string result(bufferSize, '\0');
		WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), -1, &result[0], bufferSize, nullptr, nullptr);

		return result;
	}
};
