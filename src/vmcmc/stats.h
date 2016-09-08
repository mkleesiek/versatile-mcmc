/**
 * @file
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#ifndef FMCMC_STATS_H_
#define FMCMC_STATS_H_

#include <vmcmc/sample.h>

#include <vector>
#include <unordered_map>

namespace vmcmc
{

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

    Sample& GetMode();
    Sample& GetMean();
    Sample& GetMedian(size_t paramIndex);

    Vector& GetVariance();
    Vector& GetError();
    Vector& GetRms();

    Vector& GetAutoCorrelation(size_t lag);
    Vector& GetAutoCorrelationTime();

    double GetAccRate();

private:
    const Chain& fSampleChain;
    std::pair<ptrdiff_t, ptrdiff_t> fSelectedRange;

    boost::optional<Sample> fMode;
    boost::optional<Sample> fMean;
    std::unordered_map<size_t, Sample> fMedian;

    boost::optional<Vector> fVariance;
    boost::optional<Vector> fError;
    boost::optional<Vector> fRms;

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


namespace stats
{

//double AutoCorrelation(uint64_t lag, int64_t startIndex = 0, int64_t endIndex = -1);
//double AutoCorrelation(uint64_t lag, int64_t startIndex, int64_t endIndex, const Sample& mean);
//std::vector<double> AutoCorrelationForSingleParams(uint64_t lag, int64_t startIndex = 0, int64_t endIndex = -1);
//std::vector<double> AutoCorrelationForSingleParams(uint64_t lag, int64_t startIndex, int64_t endIndex, const Sample& mean);
//
//double AutoCorrelationTime(int64_t startIndex = 0, int64_t endIndex = -1);
//std::vector<double> AutoCorrelationTimeForSingleParams(int64_t startIndex = 0, int64_t endIndex = -1);
//
//int64_t ConvergenceLength(int64_t searchInterval = -1, double threshold = 0.1, uint32_t nZeros = 2);
//std::vector<int64_t> ConvergenceLengthForSingleParams(int64_t searchInterval = -1, double threshold = 0.1, uint32_t nZeros = 2);
//
//umatrix_tri_lower_t CovarianceMatrix(int64_t startIndex = 0, int64_t endIndex = -1);
//umatrix_tri_lower_unit_t CorrelationMatrix(int64_t startIndex = 0, int64_t endIndex = -1);
//umatrix_tri_lower_t CholeskyDecomposition(int64_t startIndex = 0, int64_t endIndex = -1);
//
//std::pair<Sample, Sample> ConfidenceInterval(uint32_t iParam, const Sample& center, double level = 0.682689, int64_t startIndex = 0, int64_t endIndex = -1);
//std::pair<double, double> ConfidenceInterval(uint32_t iParam, double center, double level = 0.682689, int64_t startIndex = 0, int64_t endIndex = -1);
//
//Sample MeanAroundMaxLikelihood(double level = 0.05, int64_t startIndex = 0, int64_t endIndex = -1);
//
//uint64_t Reduce(int thinning);
//
//uint64_t CheckIndices(int64_t& startIndex, int64_t& endIndex);

template<class ContainerT>
inline std::pair<size_t, size_t>
indices(const ContainerT& c, ptrdiff_t startIndex, ptrdiff_t endIndex)
{
    const size_t N = c.size();

    if (startIndex < 0)
        startIndex = N + startIndex;
    if (endIndex < 0)
        endIndex = N + endIndex + 1;

    endIndex = std::min<size_t>(N, endIndex);
    startIndex = std::min<size_t>(N, startIndex);

    assert(startIndex >= 0 && endIndex >= startIndex);

    return std::pair<size_t, size_t>( startIndex, endIndex );
}

template<class ContainerT>
inline std::pair<typename ContainerT::const_iterator, typename ContainerT::const_iterator>
iterators(const ContainerT& c, ptrdiff_t startIndex, ptrdiff_t endIndex)
{
    const std::pair<size_t, size_t> indexPair = indices(c, startIndex, endIndex);
    return std::make_pair( std::next(std::begin(c), indexPair.first), std::next(std::begin(c), indexPair.second) );
}

} /* namespace stats */

} /* namespace vmcmc */

#endif /* FMCMC_STATS_H_ */
