/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 03.10.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/exception.hpp>
#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(Exception, What)
{
    try {
        throw Exception() << "some message";
    }
    catch (const Exception& e) {
        ASSERT_STREQ( "some message", e.what() );
    }
}

TEST(Exception, Nest)
{
    try {
        try {
            throw Exception() << "inner message";
        }
        catch (const Exception& e) {
            throw Exception().Nest(e) << "outer message";
        }
    }
    catch (const Exception& e) {
        ASSERT_STREQ( "outer message [inner message]", e.what() );
    }
}
