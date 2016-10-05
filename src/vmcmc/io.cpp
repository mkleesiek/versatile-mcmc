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
#include <vmcmc/exception.hpp>
#include <vmcmc/numeric.hpp>
#include <vmcmc/logger.hpp>

#include <iomanip>

using namespace std;

namespace vmcmc
{

LOG_DEFINE("vmcmc.io");

void Writer::Write(size_t chainIndex, const Sample& sample)
{
    Chain tmpChain;
    tmpChain.push_back( sample );
    Write(chainIndex, tmpChain, 0);
}

TextFileWriter::TextFileWriter(const string& directory, const string& stem,
        const string& separator, const string& extension) :
    fFileDirectory(directory),
    fFileStem(stem),
    fFileSeparator(separator),
    fFileExtension(extension)
{ }

TextFileWriter::~TextFileWriter()
{ }

TextFileWriter::TextFileWriter(const TextFileWriter& other) :
    Writer(other),
    fFileDirectory(other.fFileDirectory),
    fFileStem(other.fFileStem),
    fFileSeparator(other.fFileSeparator),
    fFileExtension(other.fFileExtension),
    fPrecision(other.fPrecision),
    fColumnSep(other.fColumnSep),
    fCombineChains(other.fCombineChains)
{ }

void TextFileWriter::SetFileNameScheme(const string& directory, const string& stem,
    const string& nameSeparator, const string& extension)
{
    fFileDirectory = directory;
    fFileStem = stem;
    fFileSeparator = nameSeparator;
    fFileExtension = extension;
}

string TextFileWriter::GetFilePath(int chainIndex) const
{
    ostringstream filePath;
    if (!fFileDirectory.empty())
        filePath << fFileDirectory << '/';
    filePath << fFileStem;
    if (chainIndex >= 0)
        filePath << fFileSeparator << setw(2) << setfill('0') << chainIndex;
    filePath << fFileExtension;
    return filePath.str();
}

void TextFileWriter::Initialize(size_t numberOfChains, const ParameterConfig& paramConfig)
{
    Writer::Initialize(numberOfChains, paramConfig);

    fFileStreams.clear();

    if (numberOfChains < 1)
        return;

    const size_t nFileStreams = (fCombineChains) ? 1 : numberOfChains;

    ostringstream firstLine;
    firstLine << "Generation";

    for (size_t i = 0; i < paramConfig.size(); i++) {
        firstLine << fColumnSep;
        const string& pName = paramConfig[i].GetName();
        if (pName.empty())
            firstLine << "Param." << i;
        else
            firstLine << pName;
    }

    firstLine << fColumnSep << "negLogL."
            << fColumnSep << "Likelihood"
            << fColumnSep << "Prior" << endl;

    for (size_t c = 0; c < nFileStreams; c++) {

        const string filePath = GetFilePath( (fCombineChains) ? -1 : c );

        fFileStreams.emplace_back( new ofstream(filePath, ios::trunc) );

        ofstream& fileStrm = *fFileStreams.back();

        if (!fileStrm.is_open() || fileStrm.fail())
            throw Exception() << "TextWriter target file is in error state.";

        fileStrm << firstLine.str();
        fileStrm.precision(fPrecision);
    }
}

void TextFileWriter::Write(size_t chainIndex, const Chain& chain, size_t startIndex)
{
    if (fCombineChains)
        chainIndex = 0;

    LOG_ASSERT( fFileStreams.size() > chainIndex && fFileStreams[chainIndex]
             && fFileStreams[chainIndex]->is_open(),
            "TextFileWriter is not properly initialized.");

    ofstream& fileStrm = *fFileStreams[chainIndex];

    for (size_t i = startIndex; i < chain.size(); i++) {
        const Sample& sample = chain[i];

        fileStrm << sample.GetGeneration();

        for (const double& v : sample.Values())
            fileStrm << fColumnSep << v;

        fileStrm << fColumnSep << sample.GetNegLogLikelihood();
        fileStrm << fColumnSep << sample.GetLikelihood();
        fileStrm << fColumnSep << sample.GetPrior();

    //    fFileStream << "\t" << sample.IsAccepted();

        fileStrm << endl;
    }
}

} /* namespace vmcmc */
