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
#include <cstdio>
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

    writer2.SetFileNameScheme(".", "test", "_", ".dat");
    ASSERT_EQ( "./test.dat", writer2.GetFilePath(-1) );
    ASSERT_EQ( "./test_00.dat", writer2.GetFilePath(0) );
    ASSERT_EQ( "./test_99.dat", writer2.GetFilePath(99) );
    ASSERT_EQ( "./test_101.dat", writer2.GetFilePath(101) );

    TextFileWriter writer3(".", "io-test", "_", ".dat");
    writer3.SetCombineChains(true);
    writer3.SetColumnSeparator(", ");
    writer3.SetPrecision(12);

    ParameterConfig pc;
    pc.SetParameter(0, "p1", 0.0, 1.0);
    pc.SetParameter(1, "p2", 0.0, 1.0);
    pc.SetParameter(2, "p3", 0.0, 1.0);
    pc.SetParameter(3, "p4", 0.0, 1.0);

    TextFileWriter writer4 = writer3;

    Sample s{ 0.0, 1.0, 2.0, 3.0 };

    writer4.Initialize(4, pc);
    writer4.Write(0, s );
    writer4.Write(1, s*2.0 );
    writer4.Write(2, s*3.0 );
    writer4.Write(3, s*4.0 );
    writer4.Finalize();

    vector<string> lines;
    ifstream output(writer4.GetFilePath(), ios::in);
    for ( string line; getline(output, line); ) {
        lines.push_back( move(line) );
    }

    ASSERT_EQ( 5, lines.size() );
    ASSERT_EQ( "0, 0, 3, 6, 9, -inf, 0, 0", lines[3] );

    output.close();

    remove( writer4.GetFilePath().c_str() );
}
