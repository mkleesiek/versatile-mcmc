/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 24.08.2016
 * @author marco@kleesiek.com
 *
 * @brief IO classes for writing MCMC samples to files (and reading back from)
 * or visual output.
 */

#ifndef VMCMC_IO_H_
#define VMCMC_IO_H_

#include <vmcmc/chain.hpp>
#include <vmcmc/parameter.hpp>

#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <deque>
#include <chrono>

class Gnuplot;

namespace vmcmc
{

/**
 * Abstract base class for mechanisms writing output files or producing
 * visual output.
 *
 * The virtual functions Initialize and Write are invoked by Algorithm.
 */
class Writer
{
public:
    Writer() { };
    virtual ~Writer() { };

    virtual void Initialize(size_t /*numberOfChains*/, const ParameterConfig& /*paramConfig*/) { }
    virtual void Write(size_t chainIndex, const Chain& chain, size_t startIndex) = 0;
    virtual void Finalize() { }

protected:
    size_t fCurrentIndex = 0;
};

/**
 * A writer streaming incoming samples to textfiles.
 */
class TextFileWriter : public Writer
{
public:
    TextFileWriter(const std::string& directory = "", const std::string& stem = "vmcmc",
        const std::string& nameSeparator = "-", const std::string& extension = ".txt");
    virtual ~TextFileWriter();

    void SetFileNameScheme(const std::string& directory, const std::string& stem,
        const std::string& nameSeparator = "-", const std::string& extension = ".txt");
    void SetPrecision(int prec) { fPrecision = prec; }
    void SetCombineChains(bool combine) { fCombineChains = combine; }
    void SetColumnSeparator(const std::string& sep) { fColSep = sep; }

    std::string GetFilePath(int chainIndex = -1) const;

    virtual void Initialize(size_t numberOfChains, const ParameterConfig& paramConfig) override;
    virtual void Write(size_t chainIndex, const Chain& chain, size_t startIndex) override;

private:
    TextFileWriter(const TextFileWriter& other);

    std::string fDirectory;
    std::string fStem;
    std::string fSeparator;
    std::string fExtension;

    int fPrecision = 12;
    std::string fColSep = "\t";
    bool fCombineChains = false;
    std::vector<std::unique_ptr<std::ofstream>> fFileStreams;
};

} /* namespace vmcmc */

#endif /* VMCMC_IO_H_ */
