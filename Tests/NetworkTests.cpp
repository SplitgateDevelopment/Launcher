// Unit tests for the pure backend-redirect logic (network/Redirect.h): host extraction,
// target parsing, map lookup, and URL rewriting. The WinHTTP/libcurl hooks need the game and
// are not unit tested.
#include <gtest/gtest.h>

#include <string>
#include <utility>

#include "HttpLogger.h"
#include "Redirect.h"

namespace
{

	namespace R = Network::Redirect;

	class RedirectTest : public ::testing::Test
	{
	  protected:
		void SetUp() override
		{
			Settings.NETWORK.Proxy = ProxyMode::Internal;
			Settings.NETWORK.Redirects = {{"splitgate.accelbyte.io", "127.0.0.1:5005"}};
		}
		void TearDown() override
		{
			Settings.NETWORK.Proxy = ProxyMode::Manual;
			Settings.NETWORK.Redirects.clear();
		}
	};

	TEST_F(RedirectTest, HostOfExtractsHost)
	{
		EXPECT_EQ(R::HostOf("https://splitgate.accelbyte.io/iam/v3/oauth/token"), "splitgate.accelbyte.io");
		EXPECT_EQ(R::HostOf("https://host:443/path"), "host");
		EXPECT_EQ(R::HostOf("splitgate.accelbyte.io"), "splitgate.accelbyte.io");
	}

	TEST_F(RedirectTest, SplitTargetParsesHostAndPort)
	{
		EXPECT_EQ(R::SplitTarget("127.0.0.1:5005"), std::make_pair(std::string("127.0.0.1"), 5005));
		EXPECT_EQ(R::SplitTarget("example.com"), std::make_pair(std::string("example.com"), 80));
	}

	TEST_F(RedirectTest, RewriteUrlRedirectsAndDowngradesToHttp)
	{
		EXPECT_EQ(R::RewriteUrl("https://splitgate.accelbyte.io/iam/v3/oauth/token"),
				  "http://127.0.0.1:5005/iam/v3/oauth/token");
	}

	TEST_F(RedirectTest, RewriteUrlLeavesUnknownHostsUntouched)
	{
		EXPECT_EQ(R::RewriteUrl("https://example.com/x"), "https://example.com/x");
	}

	TEST_F(RedirectTest, RewriteUrlNoopWhenDisabled)
	{
		Settings.NETWORK.Proxy = ProxyMode::Manual;
		EXPECT_EQ(R::RewriteUrl("https://splitgate.accelbyte.io/x"), "https://splitgate.accelbyte.io/x");
	}

	TEST_F(RedirectTest, IsRedirectHostIgnoresEnabledFlag)
	{
		Settings.NETWORK.Proxy = ProxyMode::Manual; // logging filter works even with redirect off
		EXPECT_TRUE(R::IsRedirectHost("https://splitgate.accelbyte.io/x"));
		EXPECT_FALSE(R::IsRedirectHost("https://example.com/x"));
	}

	TEST_F(RedirectTest, RewriteUrlPreservesPathAndQuery)
	{
		EXPECT_EQ(R::RewriteUrl("https://splitgate.accelbyte.io/a/b?c=d&e=f"), "http://127.0.0.1:5005/a/b?c=d&e=f");
	}

	TEST_F(RedirectTest, RewriteUrlHandlesNoPathAndHttpScheme)
	{
		EXPECT_EQ(R::RewriteUrl("https://splitgate.accelbyte.io"), "http://127.0.0.1:5005");
		EXPECT_EQ(R::RewriteUrl("http://splitgate.accelbyte.io/x"), "http://127.0.0.1:5005/x");
	}

	TEST_F(RedirectTest, MultipleRedirectsResolveIndependently)
	{
		Settings.NETWORK.Redirects = {{"a.example", "1.1.1.1:1"}, {"b.example", "2.2.2.2:2"}};
		EXPECT_EQ(R::RewriteUrl("https://a.example/x"), "http://1.1.1.1:1/x");
		EXPECT_EQ(R::RewriteUrl("https://b.example/y"), "http://2.2.2.2:2/y");
		EXPECT_EQ(R::RewriteUrl("https://c.example/z"), "https://c.example/z");
	}

	/// Tests for the HTTP request-flow buffer (no OS/HTTP-stack deps).
	class HttpLoggerTest : public ::testing::Test
	{
	  protected:
		void SetUp() override
		{
			Settings.NETWORK.HttpLogging = true;
			Settings.NETWORK.HttpLogToFile = false;
			Settings.NETWORK.HttpLogRedirectedOnly = false;
			Settings.NETWORK.Redirects = {{"splitgate.accelbyte.io", "127.0.0.1:5005"}};
			Network::Http::Clear();
		}
		void TearDown() override
		{
			Settings.NETWORK.HttpLogging = false;
			Settings.NETWORK.HttpLogRedirectedOnly = false;
			Settings.NETWORK.Redirects.clear();
			Network::Http::Clear();
		}
	};

	TEST_F(HttpLoggerTest, RecordsWhenEnabled)
	{
		Network::Http::Log("GET", "http://x/y");
		const auto recent = Network::Http::Recent();
		ASSERT_EQ(recent.size(), 1u);
		EXPECT_EQ(recent.front(), "GET http://x/y");
	}

	TEST_F(HttpLoggerTest, SkipsWhenDisabled)
	{
		Settings.NETWORK.HttpLogging = false;
		Network::Http::Log("GET", "http://x/y");
		EXPECT_TRUE(Network::Http::Recent().empty());
	}

	TEST_F(HttpLoggerTest, RedirectedOnlyFiltersUnknownHosts)
	{
		Settings.NETWORK.HttpLogRedirectedOnly = true;
		Network::Http::Log("GET", "https://example.com/x");				// not a redirect key
		Network::Http::Log("POST", "https://splitgate.accelbyte.io/t"); // is a redirect key
		const auto recent = Network::Http::Recent();
		ASSERT_EQ(recent.size(), 1u);
		EXPECT_EQ(recent.front(), "POST https://splitgate.accelbyte.io/t");
	}

	TEST_F(HttpLoggerTest, BufferIsCappedAndClearable)
	{
		for (int i = 0; i < static_cast<int>(Network::Http::maxRecent) + 5; ++i)
			Network::Http::Log("", "http://h/" + std::to_string(i));
		EXPECT_EQ(Network::Http::Recent().size(), Network::Http::maxRecent);

		Network::Http::Clear();
		EXPECT_TRUE(Network::Http::Recent().empty());
	}

} // namespace
