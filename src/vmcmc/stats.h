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
#include <type_traits>

namespace vmcmc
{

namespace stats
{

template<class XContainerT>
Sample mean(const XContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class XContainerT>
Sample rms(const XContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class XContainerT>
Sample median(const XContainerT& c, typename XContainerT::value_type::size_type paramIndex, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class XContainerT>
Sample variance(const XContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class XContainerT>
Sample error(const XContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class XContainerT>
Sample maxl(const XContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);

template<class XContainerT>
double accRate(const XContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);


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

template<class XContainerT>
std::pair<typename XContainerT::const_iterator, typename XContainerT::const_iterator>
iterators(const XContainerT& c, ptrdiff_t startIndex = 0, ptrdiff_t endIndex = -1);





template<class XContainerT>
inline double accRate(const XContainerT& c, ptrdiff_t startIndex, ptrdiff_t endIndex)
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

template<class XContainerT>
inline std::pair<typename XContainerT::const_iterator, typename XContainerT::const_iterator>
iterators(const XContainerT& c, ptrdiff_t startIndex, ptrdiff_t endIndex)
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

};

} /* namespace vmcmc */

#endif /* FMCMC_STATS_H_ */
