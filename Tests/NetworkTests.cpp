// Unit tests for the pure backend-redirect logic (network/Redirect.h): host extraction,
// target parsing, map lookup, and URL rewriting. The WinHTTP/libcurl hooks need the game and
// are not unit tested.
#include <gtest/gtest.h>

#include <string>
#include <utility>

#include "Redirect.h"

namespace
{

	namespace R = Network::Redirect;

	class RedirectTest : public ::testing::Test
	{
	  protected:
		void SetUp() override
		{
			Settings.NETWORK.RedirectEnabled = true;
			Settings.NETWORK.Redirects = {{"splitgate.accelbyte.io", "127.0.0.1:5005"}};
		}
		void TearDown() override
		{
			Settings.NETWORK.RedirectEnabled = false;
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
		Settings.NETWORK.RedirectEnabled = false;
		EXPECT_EQ(R::RewriteUrl("https://splitgate.accelbyte.io/x"), "https://splitgate.accelbyte.io/x");
	}

	TEST_F(RedirectTest, IsRedirectHostIgnoresEnabledFlag)
	{
		Settings.NETWORK.RedirectEnabled = false; // logging filter works even with redirect off
		EXPECT_TRUE(R::IsRedirectHost("https://splitgate.accelbyte.io/x"));
		EXPECT_FALSE(R::IsRedirectHost("https://example.com/x"));
	}

} // namespace
