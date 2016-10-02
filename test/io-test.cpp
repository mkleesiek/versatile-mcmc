/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 24.08.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/io.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace vmcmc;

TEST(IO, TextFileWriter)
{
    TextFileWriter writer1("", "vmcmc");
    ASSERT_EQ( "vmcmc.txt", writer1.GetFilePath(-1) );

    TextFileWriter writer2(".", "test", "_", ".dat");
    ASSERT_EQ( "./test.dat", writer2.GetFilePath(-1) );
    ASSERT_EQ( "./test_00.dat", writer2.GetFilePath(0) );
    ASSERT_EQ( "./test_99.dat", writer2.GetFilePath(99) );
    ASSERT_EQ( "./test_101.dat", writer2.GetFilePath(101) );
}
