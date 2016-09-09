/**
 * @file chain.h
 *
 * @date 09.09.2016
 * @author marco@kleesiek.com
 * @description
 */
#ifndef SRC_VMCMC_CHAIN_HPP_
#define SRC_VMCMC_CHAIN_HPP_

#include <vmcmc/blas.hpp>
#include <vmcmc/sample.hpp>

#include <vector>
#include <deque>
#include <unordered_map>

#include <boost/optional.hpp>

namespace vmcmc
{

using Chain = std::deque<Sample>;

/**
 * Calculates statistical momenta and properities for a specific Chain of
 * Samples.
 * All quantities are cached and calculated only once, unless they are reset
 * by calling Reset() or re-adjusting the selected window of the chain with
 * SelectRange().
 */
class ChainStats
{
public:
    ChainStats(const Chain& sampleChain);

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
 * Manages a list of ChainStats and calculates statistical properties
 * concerning sets of individual chains.
 */
class ChainSetStats
{
public:
    const std::vector<ChainStats>& GetListOfChainStats() const { return fSingleChainStats; }

    ChainStats& AddChain(const Chain& sampleChain);
    ChainStats& GetChainStats(size_t index) { return fSingleChainStats[index]; }
    size_t Size() const { return fSingleChainStats.size(); }
    void ClearChains();

    void Reset();

    void SelectRange(ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);
    void SelectPercentageRange(double start = 0.0, double end = 1.0);

    double GetRubinGelman();

private:
    std::vector<ChainStats> fSingleChainStats;

    boost::optional<double> fRubinGelman;
};

inline size_t ChainStats::NumberOfParams() const
{
    return (fSampleChain.empty()) ? 0 : fSampleChain.front().Values().size();
}

} /* namespace vmcmc */

#endif /* SRC_VMCMC_CHAIN_HPP_ */
