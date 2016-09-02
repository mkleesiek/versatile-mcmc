/**
 * @file
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 */

#ifndef FMCMC_STATS_H_
#define FMCMC_STATS_H_

#include <vmcmc/sample.h>

#include <deque>
#include <vector>
#include <type_traits>

namespace vmcmc
{

template<class ContainerT = Chain>
class ChainStatistics
{
public:
    ChainStatistics(const ContainerT& sampleChain) :
        fSampleChain( sampleChain ) { }

private:
    const ContainerT& fSampleChain;
};

template<class ContainerT = Chain>
class ChainSetStatistics
{
public:

private:
    std::vector<ChainStatistics<ContainerT>> fSingleChainStats;
};

namespace stats
{

template<class ContainerT>
Sample mean(const ContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class ContainerT>
Sample rms(const ContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class ContainerT>
Sample median(const ContainerT& c, typename ContainerT::value_type::size_type paramIndex, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class ContainerT>
Sample variance(const ContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class ContainerT>
Sample error(const ContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class ContainerT>
Sample maxl(const ContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class ContainerT>
double accRate(const ContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);


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
std::pair<typename ContainerT::const_iterator, typename ContainerT::const_iterator>
iterators(const ContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);


template<class ContainerT>
inline double accRate(const ContainerT& c, ptrdiff_t startIndex, ptrdiff_t endIndex)
{
    auto itRange = iterators(c, startIndex, endIndex);

    // skip the first element
    if (itRange.first != itRange.second)
        std::advance(itRange.first, 1);

    size_t accepted = 0;
    size_t total = 0;

    for (auto it = itRange.first; it != itRange.second; it++) {
        if (it->IsAccepted())
            accepted++;
        total++;
    }

    return (total == 0) ? 0.0 : (double) accepted / (double) total;
}

template<class ContainerT>
inline std::pair<typename ContainerT::const_iterator, typename ContainerT::const_iterator>
iterators(const ContainerT& c, ptrdiff_t startIndex, ptrdiff_t endIndex)
{
    const size_t N = c.size();

    if (startIndex < 0)
        startIndex = N + startIndex;
    if (endIndex < 0)
        endIndex = N + endIndex + 1;

    endIndex = std::min<size_t>(N, endIndex);
    startIndex = std::min<size_t>(N, startIndex);

    assert(startIndex >= 0 && endIndex >= startIndex);

    return std::make_pair( std::next(std::begin(c), startIndex), std::next(std::begin(c), endIndex) );
}

//double KFMCDream::GelmanRubinDiagnostic() const
//{
//    const uint64_t nGens = NumberOfGenerations();
//
//    if (nGens < 10)
//        return 0;
//
//    const uint32_t d = NumberOfParameters();
//
//    vector<vector<AccumulatorVariance_t> > accus ( fN, vector<AccumulatorVariance_t>(d) );
//
//    vector<KFMCState> fChainsMeans( fN, KFMCState(d) );
//    vector<KFMCState> fChainsVariances( fN, KFMCState(d) );
//    KFMCState fGlobalMean(d);
//
//    // for each chain
//    for (uint32_t i = 0; i < fN; ++i) {
//
//        vector<KFMCState>::const_iterator it = fChains[i].Data().cbegin();
//        // accumulate variance and mean within the last 50% of the samples
//        advance(it, (nGens / 2) );
//
//        for (; it != fChains[i].Data().cend(); ++it) {
//            for (uint32_t j = 0; j < d; ++j) {
//                accus[i][j]( it->Parameter(j) );
//            }
//        }
//
//        // extract
//        for (uint32_t j = 0; j < d; ++j) {
//            fChainsMeans[i].Parameter(j) = bac::mean( accus[i][j] );
//            fChainsVariances[i].Parameter(j) = bac::variance_unbiased( accus[i][j] );
//        }
//
//        fGlobalMean += fChainsMeans[i];
//    }
//
//    fGlobalMean /= (double) fN;
//
//    vector<double> R (d, 0.0);
//
//    // number of points taken into account
//    const uint64_t n = bac::count( accus[0][0] );
//
//    // for each dimension
//    for (uint32_t j = 0; j < d; ++j) {
//
//        // variance between chains:
//        double B = 0.0;
//        // variance within chains:
//        double W = 0.0;
//
//        for (uint32_t i = 0; i < fN; ++i) {
//            B += kpow<2>( fChainsMeans[i].Parameter(j) - fGlobalMean.Parameter(j) );
//            W += fChainsVariances[i].Parameter(j);
//        }
//        B /= (double) (fN - 1);
//        W /= (double) fN;
//
//        const double V = ((double) (n - 1) / (double) n) * W + B + (B / (double) fN);
//        R[j] = V / W;
//    }
//
//    return *max_element(R.begin(), R.end());
//}


};

} /* namespace vmcmc */

#endif /* FMCMC_STATS_H_ */
