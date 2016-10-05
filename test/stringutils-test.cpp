/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 02.10.2016
 * @author marco@kleesiek.com
 */

#include <gtest/gtest.h>

#include <vmcmc/stringutils.hpp>
#include <vmcmc/sample.hpp>

#include <vector>
#include <deque>
#include <map>

using namespace std;
using namespace vmcmc;

TEST(StringUtils, join)
{
    vector<int> intVector = { 1, 2, 3, 4, 5 };
    vector<double> doubleVector = { 1.1, 2.02, 3.003, 4.0004, 5.00005} ;

    ostringstream testStrm;
    join(testStrm, intVector, ">>");
    ASSERT_EQ( "1>>2>>3>>4>>5", testStrm.str() );

    ASSERT_EQ( "1, 2, 3, 4, 5", join(intVector, ", ", 5) );

    ASSERT_EQ( "1.1, 2.02, 3.003, 4.0004, 5", join(doubleVector, ", ", 5) );
}

TEST(StringUtils, Sample)
{
    ostringstream testStrm;
    Sample testSample{ 1, 2, 3, 4 };
    testStrm << testSample;
    ASSERT_EQ( "[4](1, 2, 3, 4) 0 (0, -inf)", testStrm.str() );
}

TEST(StringUtils, Deque)
{
    deque<string> strDeque = { "foo", "bar" };
    ostringstream testStrm;
    testStrm << strDeque;
    ASSERT_EQ( "[2](foo, bar)", testStrm.str() );
}

TEST(StringUtils, Map)
{
    map<string, double> testMap = { {"foo", 3.0}, {"bar", 99.99} };
    ostringstream testStrm;
    testStrm << testMap;
    ASSERT_EQ( "[2]((bar, 99.99), (foo, 3))", testStrm.str() );
}
