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
protected:
    Writer() { };
    virtual ~Writer() { };

public:
    virtual void Initialize(size_t /*numberOfChains*/, const ParameterConfig& /*paramConfig*/) { }
    virtual void Write(size_t chainIndex, const Chain& chain, size_t startIndex) = 0;
    virtual void Finalize() { }

    void Write(size_t chainIndex, const Sample& sample);
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

    TextFileWriter(const TextFileWriter& other);

    void SetFileNameScheme(const std::string& directory, const std::string& stem,
        const std::string& nameSeparator = "-", const std::string& extension = ".txt");
    void SetPrecision(int prec) { fPrecision = prec; }
    void SetCombineChains(bool combine) { fCombineChains = combine; }
    void SetColumnSeparator(const std::string& sep) { fColumnSep = sep; }

    std::string GetFilePath(int chainIndex = -1) const;

    virtual void Initialize(size_t numberOfChains, const ParameterConfig& paramConfig) override;
    virtual void Write(size_t chainIndex, const Chain& chain, size_t startIndex) override;

    using Writer::Write;

    std::string fFileDirectory;
    std::string fFileStem;
    std::string fFileSeparator;
    std::string fFileExtension;

    int fPrecision = 12;
    std::string fColumnSep = "\t";
    bool fCombineChains = false;
    std::vector<std::unique_ptr<std::ofstream>> fFileStreams;
};

///**
// * Near-time graphic visualization of each chain's evolution.
// *
// * HIGHLY EXPERIMENTAL.
// */
//class GnuplotWriter: public Writer
//{
//public:
//    GnuplotWriter();
//    virtual ~GnuplotWriter();
//
//    virtual void Initialize(size_t numberOfChains, const ParameterConfig& paramConfig) override;
//    virtual void Write(size_t chainIndex, const Chain& chain, size_t startIndex) override;
//    virtual void Finalize() override;
//
//protected:
//    void Replot(bool force = false);
//
//private:
//    ParameterConfig fParameterConfig;
//    size_t fNumberOfChains = 0;
//
//    std::vector<std::unique_ptr<Gnuplot>> fGnuplotWindows;
//
//    std::vector<std::deque<double>> fGenerationBuffers;
//    std::vector<std::vector<std::deque<double>>> fValueBuffers;
//    size_t fMaxBufferSize = 300;
//
//    int fRefreshRateInMs = 5000;
//    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> fLastPlotTime;
//};

} /* namespace vmcmc */

#endif /* VMCMC_IO_H_ */
