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
#include <vmcmc/typetraits.hpp>
#include <vmcmc/chain.hpp>

#include <vector>
#include <set>
#include <map>
#include <string>

using namespace std;
using namespace vmcmc;

TEST(TypeTraits, is_container)
{
    ASSERT_TRUE( is_container<vector<bool>>::value );
    ASSERT_TRUE( is_container<vector<int>>::value );
    ASSERT_TRUE( is_container<set<double>>::value );
    ASSERT_TRUE( (is_container<map<string, string>>::value) );
    ASSERT_TRUE( is_container<Chain>::value );

    ASSERT_FALSE( is_container<string>::value );
}
