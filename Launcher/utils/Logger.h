#pragma once

#include <iostream>
#include <format>
#include <Windows.h>
#include <strsafe.h>

struct logDTO {
	std::string text;
};

class Logger {
public:
	void error(std::string message) {
		return _log({
			"ERROR",
		}, message);
	};

	void success(std::string message) {
		return _log({
			"SUCCESS",
			}, message);
	};

	void info(std::string message) {
		return _log({
			"INFO",
			}, message);
	};

	//https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
	void errorBox(LPCTSTR lpszFunction) {
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		// Display the error message and exit the process

		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
			(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
		StringCchPrintf((LPTSTR)lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("%s failed with error %d: %s"),
			lpszFunction, dw, lpMsgBuf);
		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);
	}

	int stop(int code) {
		info("Press any key to exit...");
		std::cin.get();
		return code;
	};

private:
	void _log(logDTO logDTO, std::string message) {

		std::cout << format("[{}] ", logDTO.text) << message << std::endl;
	};
};