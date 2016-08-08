/*
 * chain.h
 *
 *  Created on: 05.08.2016
 *      Author: Marco Kleesiek <marco@kleesiek.com>
 */

#ifndef FMCMC_STATS_H_
#define FMCMC_STATS_H_

#include <fmcmc/sample.h>

#include <deque>
#include <type_traits>

namespace fmcmc
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
//std::pair<const_iterator, const_iterator> GetIterators(int64_t startIndex, int64_t endIndex);

};

} /* namespace fmcmc */

#endif /* FMCMC_STATS_H_ */
