/**
 * @file io.cpp
 *
 * @date 24.08.2016
 * @author marco@kleesiek.com
 * @description
 */

#include <vmcmc/exception.h>
#include <vmcmc/io.h>
#include <vmcmc/numeric.h>

#include <iomanip>

using namespace std;

namespace vmcmc
{

AsciiWriter::AsciiWriter(const string& directory, const string& stem,
        const string& separator, const string& extension) :
    fDirectory(directory),
    fStem(stem),
    fSeparator(separator),
    fExtension(extension)
{ }

AsciiWriter::~AsciiWriter()
{
    fFileStream.close();
}

AsciiWriter::AsciiWriter(const AsciiWriter& other) :
    Writer(other),
    fDirectory(other.fDirectory),
    fStem(other.fStem),
    fSeparator(other.fSeparator),
    fExtension(other.fExtension)
{ }

AsciiWriter* AsciiWriter::Clone() const
{
    return new AsciiWriter(*this);
}

bool AsciiWriter::Initialize(size_t chainIndex, size_t totalNChains)
{
    if (fFileStream.is_open())
        throw Exception() << "TextWriter target file is already open.";

    ostringstream filePath;
    filePath << fDirectory << '/' << fStem;
    if (totalNChains > 0) {
        filePath << fSeparator
            << setw( numeric::numberOfDigits(totalNChains) ) << setfill('0')
            << chainIndex;
    }
    filePath << fExtension;

    fFileStream.open(filePath.str(), ios::trunc);

    return (fFileStream.is_open() && fFileStream.good());
}

void AsciiWriter::Write(const Sample& sample)
{
    if (!fFileStream.is_open() || fFileStream.fail())
        throw Exception() << "TextWriter target file is in error state.";

    fFileStream.precision(fPrecision);
    fFileStream.fill('\t');

    fFileStream << sample.GetGeneration();

    for (const double& v : sample.Values())
        fFileStream << v;

    fFileStream << sample.GetNegLogLikelihood();
    fFileStream << sample.GetLikelihood();
    fFileStream << sample.GetPrior();

    fFileStream << sample.IsAccepted();

    fFileStream << endl;
}

} /* namespace vmcmc */
