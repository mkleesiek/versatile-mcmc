/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 09.09.2016
 * @author marco@kleesiek.com
 *
 * @brief Utility classes for calculation statistical momenta and diagnostics.
 */
#ifndef SRC_VMCMC_CHAIN_HPP_
#define SRC_VMCMC_CHAIN_HPP_

#include <vmcmc/blas.hpp>
#include <vmcmc/sample.hpp>

#include <vector>
#include <unordered_map>

#include <boost/optional.hpp>

namespace vmcmc
{

using Chain = std::vector<Sample>;

/**
 * Calculates statistical momenta and properties for a given Chain of Samples.
 *
 * All quantities are cached and calculated only once, unless they are reset
 * by calling Reset() or re-adjusting the selected window of the chain with
 * SelectRange().
 */
class ChainStatistics
{
public:
    ChainStatistics(const Chain& sampleChain);

    const Chain& GetChain() const { return fSampleChain; }

    void Reset();

    void SelectRange(ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);
    void SelectPercentageRange(double start = 0.0, double end = 1.0);

    std::pair<size_t, size_t> GetIndices() const;
    std::pair<Chain::const_iterator, Chain::const_iterator> GetIterators() const;

    size_t NumberOfParams() const;

    const Sample& GetMode();
    Sample& GetMean();
    double GetMedian(size_t paramIndex);

    const Vector& GetVariance();
    const Vector& GetError();
    const Vector& GetRms();

    const MatrixLower& GetCovarianceMatrix();
    const MatrixUnitLower& GetCorrelationMatrix();
    const MatrixLower& GetCholeskyDecomposition();

    const Vector& GetAutoCorrelation(size_t lag);
    const Vector& GetAutoCorrelationTime();

    std::pair<double, double> GetConfidenceInterval(size_t iParam, double centralValue, double level);

    double GetAccRate();

private:
    const Chain& fSampleChain;
    std::pair<ptrdiff_t, ptrdiff_t> fSelectedRange;

    boost::optional<Sample> fMode;
    boost::optional<Sample> fMean;
    std::unordered_map<size_t, double> fMedian;

    boost::optional<Vector> fVariance;
    boost::optional<Vector> fError;
    boost::optional<Vector> fRms;

    boost::optional<MatrixLower> fCovariance;
    boost::optional<MatrixUnitLower> fCorrelation;
    boost::optional<MatrixLower> fCholesky;

    std::unordered_map<size_t, Vector> fAutoCorrelation;
    boost::optional<Vector> fAutoCorrelationTime;

    boost::optional<double> fAccRate;
};

/**
 * Manages a list of ChainStatistics and calculates statistical properties
 * regarding sets of individual chains.
 */
class ChainSetStatistics
{
public:
    const std::vector<ChainStatistics>& GetListOfChainStats() const { return fSingleChainStats; }

    ChainStatistics& AddChain(const Chain& sampleChain);
    ChainStatistics& GetChainStats(size_t index) { return fSingleChainStats[index]; }
    size_t Size() const { return fSingleChainStats.size(); }
    void ClearChains();

    void Reset();

    void SelectRange(ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);
    void SelectPercentageRange(double start = 0.0, double end = 1.0);

    /**
     * Calculates the Gelman–Rubin for all aggregated MCMC chains.
     *
     * The Gelman-Rubin diagnostic evaluates MCMC convergence by analyzing the
     * difference between multiple Markov chains (see Gelman and Rubin 1992).
     * @return The diagnostic R, which according to Brooks and Gelman (1997)
     * should be < 1.1 or 1.2 to assure convergence.
     *
     * @see Brooks, S. P., and A. Gelman. 1997. General Methods for Monitoring
     * Convergence of Iterative Simulations. Journal of Computational and
     * Graphical Statistics 7: 434–455
     * @see Gelman, A., and D. B. Rubin. 1992. Inference from Iterative
     * Simulation Using Multiple Sequences. Statistical Science 7: 457–511.
     */
    double GetGelmanRubin();

private:
    std::vector<ChainStatistics> fSingleChainStats;

    boost::optional<double> fRubinGelman;
};

inline size_t ChainStatistics::NumberOfParams() const
{
    return (fSampleChain.empty()) ? 0 : fSampleChain.front().Values().size();
}

} /* namespace vmcmc */

#endif /* SRC_VMCMC_CHAIN_HPP_ */
