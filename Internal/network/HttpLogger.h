#pragma once

#include <fstream>
#include <string>

#include "../settings/Settings.h"
#include "../utils/Logger.h"
#include "Redirect.h"

/**
 * @file
 * @brief HTTP call logging — to the console and, optionally, to `http.log`, with an optional
 * filter to only redirected hosts. Fed by the WinHTTP / libcurl hooks and gated on
 * Settings.NETWORK.HttpLogging.
 */
namespace Network::Http
{
	inline std::ofstream logFile; ///< lazily opened http.log (append mode)

	/**
	 * Logs one outgoing HTTP call, honoring HttpLogging / HttpLogRedirectedOnly / HttpLogToFile.
	 * @param method HTTP verb (may be empty when the stack doesn't expose it at this point).
	 * @param url    the full request URL.
	 */
	inline void Log(const std::string& method, const std::string& url)
	{
		if (!Settings.NETWORK.HttpLogging) return;
		if (Settings.NETWORK.HttpLogRedirectedOnly && !Redirect::IsRedirectHost(url)) return;

		const std::string line = method.empty() ? url : method + " " + url;
		Logger::Log("HTTP", line);

		if (!Settings.NETWORK.HttpLogToFile) return;

		if (!logFile.is_open())
			logFile.open(SettingsHelper::GetAppPath("http.log"), std::ios::out | std::ios::app);
		if (logFile.is_open()) logFile << line << std::endl;
	}
} // namespace Network::Http
