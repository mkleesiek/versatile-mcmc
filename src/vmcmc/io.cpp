/**
 * @file io.cpp
 *
 * @date 24.08.2016
 * @author marco@kleesiek.com
 * @description
 */

#include <exception.hpp>
#include <io.hpp>
#include <numeric.hpp>
#include <iomanip>

using namespace std;

namespace vmcmc
{

TextFileWriter::TextFileWriter(const string& directory, const string& stem,
        const string& separator, const string& extension) :
    fDirectory(directory),
    fStem(stem),
    fSeparator(separator),
    fExtension(extension)
{ }

TextFileWriter::~TextFileWriter()
{ }

TextFileWriter::TextFileWriter(const TextFileWriter& other) :
    Writer(other),
    fDirectory(other.fDirectory),
    fStem(other.fStem),
    fSeparator(other.fSeparator),
    fExtension(other.fExtension),
    fPrecision(other.fPrecision),
    fWriteCombined(other.fWriteCombined)
{ }

TextFileWriter* TextFileWriter::Clone() const
{
    return new TextFileWriter(*this);
}

string TextFileWriter::GetFilePath(int chainIndex) const
{
    ostringstream filePath;
    if (!fDirectory.empty())
        filePath << fDirectory << '/';
    filePath << fStem;
    if (chainIndex >= 0)
        filePath << fSeparator << setw(2) << setfill('0') << chainIndex;
    filePath << fExtension;
    return filePath.str();
}

void TextFileWriter::Write(size_t chainIndex, const Sample& sample)
{
    WriteFile( (int) chainIndex, sample );

    if (fWriteCombined)
        WriteFile( -1, sample );
}

void TextFileWriter::WriteFile(int streamIndex, const Sample& sample)
{
    ofstream& fileStrm = fFileStreams[streamIndex];

    if (!fileStrm.is_open()) {
        const string filePath = GetFilePath(streamIndex);
        fileStrm.open(filePath, ios::trunc);

        if (!fileStrm.is_open() || fileStrm.fail())
            throw Exception() << "TextWriter target file is in error state.";
    }

    fileStrm.precision(fPrecision);

    fileStrm << sample.GetGeneration();

    for (const double& v : sample.Values())
        fileStrm << "\t" << v;

    fileStrm << "\t" << sample.GetNegLogLikelihood();
    fileStrm << "\t" << sample.GetLikelihood();
    fileStrm << "\t" << sample.GetPrior();

//    fFileStream << "\t" << sample.IsAccepted();

    fileStrm << endl;
}

} /* namespace vmcmc */
