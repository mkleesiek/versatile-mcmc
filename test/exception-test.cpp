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
#include <iostream>

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

TEST(Exception, Copy)
{
    Exception e1;
    e1 << "someMessage";
    e1.Nest( Exception() << "innerMessage" );

    Exception e2(e1);
    ASSERT_STREQ( e1.what(), e2.what() );

    e2 = e1;
    ASSERT_STREQ( e1.what(), e2.what() );
}

TEST(Exception, Stream)
{
    Exception e1;
    e1 << "someMessage";
    e1.Nest( Exception() << "innerMessage" );

    ostringstream strm;
    strm << e1;

    ASSERT_EQ( "someMessage [innerMessage]", strm.str() );
}
