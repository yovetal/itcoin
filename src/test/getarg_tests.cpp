#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-it");
    BOOST_CHECK(GetBoolArg("-it"));
    BOOST_CHECK(GetBoolArg("-it", false));
    BOOST_CHECK(GetBoolArg("-it", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ito"));
    BOOST_CHECK(!GetBoolArg("-ito", false));
    BOOST_CHECK(GetBoolArg("-ito", true));

    ResetArgs("-it=0");
    BOOST_CHECK(!GetBoolArg("-it"));
    BOOST_CHECK(!GetBoolArg("-it", false));
    BOOST_CHECK(!GetBoolArg("-it", true));

    ResetArgs("-it=1");
    BOOST_CHECK(GetBoolArg("-it"));
    BOOST_CHECK(GetBoolArg("-it", false));
    BOOST_CHECK(GetBoolArg("-it", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noit");
    BOOST_CHECK(!GetBoolArg("-it"));
    BOOST_CHECK(!GetBoolArg("-it", false));
    BOOST_CHECK(!GetBoolArg("-it", true));

    ResetArgs("-noit=1");
    BOOST_CHECK(!GetBoolArg("-it"));
    BOOST_CHECK(!GetBoolArg("-it", false));
    BOOST_CHECK(!GetBoolArg("-it", true));

    ResetArgs("-it -noit");  // -it should win
    BOOST_CHECK(GetBoolArg("-it"));
    BOOST_CHECK(GetBoolArg("-it", false));
    BOOST_CHECK(GetBoolArg("-it", true));

    ResetArgs("-it=1 -noit=1");  // -it should win
    BOOST_CHECK(GetBoolArg("-it"));
    BOOST_CHECK(GetBoolArg("-it", false));
    BOOST_CHECK(GetBoolArg("-it", true));

    ResetArgs("-it=0 -noit=0");  // -it should win
    BOOST_CHECK(!GetBoolArg("-it"));
    BOOST_CHECK(!GetBoolArg("-it", false));
    BOOST_CHECK(!GetBoolArg("-it", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--it=1");
    BOOST_CHECK(GetBoolArg("-it"));
    BOOST_CHECK(GetBoolArg("-it", false));
    BOOST_CHECK(GetBoolArg("-it", true));

    ResetArgs("--noit=1");
    BOOST_CHECK(!GetBoolArg("-it"));
    BOOST_CHECK(!GetBoolArg("-it", false));
    BOOST_CHECK(!GetBoolArg("-it", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-it", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-it", "eleven"), "eleven");

    ResetArgs("-it -bar");
    BOOST_CHECK_EQUAL(GetArg("-it", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-it", "eleven"), "");

    ResetArgs("-it=");
    BOOST_CHECK_EQUAL(GetArg("-it", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-it", "eleven"), "");

    ResetArgs("-it=11");
    BOOST_CHECK_EQUAL(GetArg("-it", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-it", "eleven"), "11");

    ResetArgs("-it=eleven");
    BOOST_CHECK_EQUAL(GetArg("-it", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-it", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-it", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-it", 0), 0);

    ResetArgs("-it -bar");
    BOOST_CHECK_EQUAL(GetArg("-it", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-it=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-it", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-it=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-it", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--it");
    BOOST_CHECK_EQUAL(GetBoolArg("-it"), true);

    ResetArgs("--it=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-it", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noit");
    BOOST_CHECK(!GetBoolArg("-it"));
    BOOST_CHECK(!GetBoolArg("-it", true));
    BOOST_CHECK(!GetBoolArg("-it", false));

    ResetArgs("-noit=1");
    BOOST_CHECK(!GetBoolArg("-it"));
    BOOST_CHECK(!GetBoolArg("-it", true));
    BOOST_CHECK(!GetBoolArg("-it", false));

    ResetArgs("-noit=0");
    BOOST_CHECK(GetBoolArg("-it"));
    BOOST_CHECK(GetBoolArg("-it", true));
    BOOST_CHECK(GetBoolArg("-it", false));

    ResetArgs("-it --noit");
    BOOST_CHECK(GetBoolArg("-it"));

    ResetArgs("-noit -it"); // it always wins:
    BOOST_CHECK(GetBoolArg("-it"));
}

BOOST_AUTO_TEST_SUITE_END()
