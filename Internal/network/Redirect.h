#pragma once

#include <optional>
#include <string>
#include <utility>

#include "../settings/Settings.h"

/**
 * @file
 * @brief Pure backend-redirect logic — host/URL lookups against the configured redirect map
 * (Settings.NETWORK.Redirects). No OS or HTTP-stack dependencies, so it is unit-testable and
 * shared by every hook (WinHTTP, libcurl).
 */
namespace Network::Redirect
{
	/// Splits a `"host"` or `"host:port"` redirect target; port defaults to 80 when absent or
	/// malformed.
	inline std::pair<std::string, int> SplitTarget(const std::string& target)
	{
		const auto colon = target.find(':');
		if (colon == std::string::npos) return {target, 80};

		const int port = std::atoi(target.c_str() + colon + 1);
		return {target.substr(0, colon), port > 0 ? port : 80};
	}

	/// Extracts the host from a URL (`scheme://host[:port]/path`), or from a bare host.
	inline std::string HostOf(const std::string& url)
	{
		auto start = url.find("://");
		start = (start == std::string::npos) ? 0 : start + 3;

		const auto end = url.find_first_of(":/", start);
		return url.substr(start, end == std::string::npos ? std::string::npos : end - start);
	}

	/// @return the redirect target (`"host[:port]"`) for @p host, or nullopt when redirection
	/// is off or @p host isn't a key in the map.
	inline std::optional<std::string> Target(const std::string& host)
	{
		if (Settings.NETWORK.Proxy != ProxyMode::Internal) return std::nullopt;

		const auto it = Settings.NETWORK.Redirects.find(host);
		if (it == Settings.NETWORK.Redirects.end()) return std::nullopt;
		return it->second;
	}

	/// @return true if @p url's host is a redirect key (used by the HTTP-log filter). Ignores
	/// the proxy mode, so a logging-only setup can still filter by the map.
	inline bool IsRedirectHost(const std::string& url)
	{
		return Settings.NETWORK.Redirects.count(HostOf(url)) > 0;
	}

	/**
	 * Rewrites a full URL for URL-based stacks (libcurl). If the URL's host is a redirect key,
	 * returns `http://<target>` + the original path (https is downgraded to http, since the
	 * private server speaks plain HTTP); otherwise returns @p url unchanged.
	 */
	inline std::string RewriteUrl(const std::string& url)
	{
		const auto target = Target(HostOf(url));
		if (!target) return url;

		const auto schemeEnd = url.find("://");
		const auto hostStart = (schemeEnd == std::string::npos) ? 0 : schemeEnd + 3;
		const auto pathStart = url.find('/', hostStart);
		const std::string path = (pathStart == std::string::npos) ? "" : url.substr(pathStart);

		return "http://" + *target + path;
	}
} // namespace Network::Redirect
