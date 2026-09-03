#pragma once

#include <deque>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

#include "../settings/Settings.h"
#include "../utils/Logger.h"
#include "Redirect.h"

/**
 * @file
 * @brief HTTP call logging — to the console, an optional `http.log` file, and an in-memory
 * ring buffer the Network tab renders as a live request flow. All sinks share the
 * HttpLogging / HttpLogRedirectedOnly gates; the file adds HttpLogToFile. Fed by the
 * WinHTTP / libcurl hooks (called from network threads, so the buffer is mutex-guarded).
 */
namespace Network::Http
{
	inline std::ofstream logFile; ///< lazily opened http.log (append mode)

	constexpr size_t maxRecent = 200;	   ///< cap on the in-memory request-flow buffer
	inline std::mutex recentMutex;		   ///< guards @ref recent
	inline std::deque<std::string> recent; ///< most recent logged request lines (for the GUI)

	/**
	 * Logs one outgoing HTTP call, honoring HttpLogging / HttpLogRedirectedOnly / HttpLogToFile.
	 * Records it in the request-flow buffer and the console (and file, if enabled).
	 * @param method HTTP verb (may be empty when the stack doesn't expose it at this point).
	 * @param url    the full request URL.
	 */
	inline void Log(const std::string& method, const std::string& url)
	{
		if (!Settings.NETWORK.HttpLogging) return;
		if (Settings.NETWORK.HttpLogRedirectedOnly && !Redirect::IsRedirectHost(url)) return;

		const std::string line = method.empty() ? url : method + " " + url;

		{
			std::lock_guard lock(recentMutex);
			recent.push_back(line);
			if (recent.size() > maxRecent) recent.pop_front();
		}

		Logger::Log("HTTP", line);

		if (!Settings.NETWORK.HttpLogToFile) return;

		if (!logFile.is_open())
			logFile.open(SettingsHelper::GetAppPath("http.log"), std::ios::out | std::ios::app);
		if (logFile.is_open()) logFile << line << std::endl;
	}

	/// @return a snapshot of the request-flow buffer (oldest first), for the GUI.
	inline std::vector<std::string> Recent()
	{
		std::lock_guard lock(recentMutex);
		return {recent.begin(), recent.end()};
	}

	/// Clears the request-flow buffer.
	inline void Clear()
	{
		std::lock_guard lock(recentMutex);
		recent.clear();
	}
} // namespace Network::Http
