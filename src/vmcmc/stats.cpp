/**
 * @file
 *
 * @date 03.09.2016
 * @author marco@kleesiek.com
 * @description
 */

#include <vmcmc/stats.h>

#include <algorithm>
#include <set>

using namespace std;

namespace vmcmc {

ChainStats::ChainStats(const Chain& sampleChain) :
    fSampleChain( sampleChain ),
    fSelectedRange{ 0, -1 }
{ }

void ChainStats::Reset()
{
    fMode.reset();
    fMean.reset();
    fMedian.clear();

    fVariance.reset();
    fError.reset();
    fRms.reset();

    fAccRate.reset();
}

void ChainStats::SelectRange(ptrdiff_t startIndex, ptrdiff_t endIndex)
{
    auto oldRange = fSelectedRange;
    fSelectedRange = make_pair( startIndex, endIndex );

    if (oldRange != fSelectedRange)
        Reset();
}

void ChainStats::SelectPercentageRange(double start, double end)
{
    const double N = fSampleChain.size();
    SelectRange( (ptrdiff_t) (N * start), (ptrdiff_t) ((N-1.0)*end)  );
}

pair<size_t, size_t> ChainStats::GetIndices() const
{
    return stats::indices(fSampleChain, fSelectedRange.first, fSelectedRange.second);
}

pair<Chain::const_iterator, Chain::const_iterator> ChainStats::GetIterators() const
{
    return stats::iterators(fSampleChain, fSelectedRange.first, fSelectedRange.second);
}

Sample& ChainStats::GetMode()
{
    if (fMode)
        return fMode.get();

    auto itRange = GetIterators();

    Sample result( NumberOfParams() );

    if (itRange.first != itRange.second) {
        result = *min_element( itRange.first, itRange.second,
            [](const Sample& s1, const Sample& s2) {
                return s1.GetNegLogLikelihood() < s2.GetNegLogLikelihood();
            }
        );
    }

    fMode = result;
    return fMode.get();
}

Sample& ChainStats::GetMean()
{
    if (fMean)
        return fMean.get();

    auto itRange = GetIterators();
    const size_t N = itRange.second - itRange.first;

    Sample result( NumberOfParams() );

    if (N > 0) {
        for (auto it = itRange.first; it != itRange.second; ++it)
            result += *it;

        result /= (double) N;
    }

    fMean = result;
    return fMean.get();
}

Sample& ChainStats::GetMedian(size_t paramIndex)
{
    auto mapIt = fMedian.find(paramIndex);

    if (mapIt != fMedian.end())
        return mapIt->second;

    auto itRange = GetIterators();
    const size_t N = itRange.second - itRange.first;

    Sample& result = fMedian.emplace( paramIndex, Sample(NumberOfParams()) ).first->second;

    if (N > 0) {
        using SampleRef = reference_wrapper<const Sample>;

        auto cmp = [=](const SampleRef& s1, const SampleRef& s2) -> bool {
            return s1.get().Values()[paramIndex] < s2.get().Values()[paramIndex];
        };

        // Construct a view of all samples, then sort with respect to the
        // selected parameter using the selection algorith 'nth_element'.

        vector<SampleRef> test( itRange.first, itRange.second );
        auto middleIt = next( test.begin(), (N/2) );
        nth_element( test.begin(), middleIt, test.end(), cmp);
        result = middleIt->get();
    }

    return result;
}

Vector& ChainStats::GetVariance()
{
    if (fVariance)
        return fVariance.get();

    auto itRange = GetIterators();
    const size_t N = itRange.second - itRange.first;

    const Sample& mean = GetMean();

    Vector result( NumberOfParams() );

    if (N > 1) {
        for (auto it = itRange.first; it != itRange.second; it++)
            for (size_t p = 0; p < result.size(); p++)
                result[p] += math::pow<2>( it->Values()[p] - mean.Values()[p] );

        result /= (double) (N-1);
    }

    fVariance = result;
    return fVariance.get();
}

Vector& ChainStats::GetError()
{
    if (fError)
        return fError.get();

    Vector result = GetVariance();

    for (double& p : result)
        p = sqrt(p);

    fError = result;
    return fError.get();
}

Vector& ChainStats::GetRms()
{
    if (fRms)
        return fRms.get();

    auto itRange = GetIterators();
    const size_t N = itRange.second - itRange.first;

    Vector result( NumberOfParams() );

    if (N > 0) {
        for (auto it = itRange.first; it != itRange.second; it++)
            for (size_t p = 0; p < result.size(); p++)
                result[p] += math::pow<2>( it->Values()[p] );

        result /= (double) N;

        for (size_t p = 0; p < result.size(); p++)
            result[p] = sqrt( result[p] );
    }

    fRms = result;
    return fRms.get();
}

Vector& ChainStats::GetAutoCorrelation(size_t lag)
{
    auto mapIt = fAutoCorrelation.find(lag);

    if (mapIt != fAutoCorrelation.end())
        return mapIt->second;

    auto itRange = GetIterators();
    const size_t N = itRange.second - itRange.first;

    Vector& result = fAutoCorrelation.emplace( lag, Vector(NumberOfParams()) ).first->second;

    if (lag < N) {

        const Sample& mean = GetMean();
        const Vector& variance = GetVariance();

        const size_t nParams = NumberOfParams();

        vector<double> nominators(nParams, 0.0);
        vector<double> var_t(nParams, 0.0);
        vector<double> var_h(nParams, 0.0);

        auto it_t = itRange.first;
        auto it_h = next(it_t, lag);

        for (; it_h != itRange.second; ++it_t, ++it_h) {
            for (size_t p = 0; p < nParams; ++p) {
                const double X_t_diff = it_t->Values()[p] - mean.Values()[p];
                const double X_h_diff = it_h->Values()[p] - mean.Values()[p];
                result[p] += X_t_diff * X_h_diff;
            }
        }

        result /= (double) (N-lag);

        for (size_t p = 0; p < result.size(); p++)
            result[p] /= variance[p];

    }

    return result;
}

Vector& ChainStats::GetAutoCorrelationTime()
{
    if (fAutoCorrelationTime)
        return fAutoCorrelationTime.get();

    auto itRange = GetIterators();
    const size_t N = itRange.second - itRange.first;

    Vector result(NumberOfParams(), 0.0);

    size_t cThresholdMaintained = 0;

    for (size_t lag = 1; lag < N; lag++) {
        const Vector& ac = GetAutoCorrelation(lag);
        result += ac;

        if (ublas::norm_inf(ac) < 0.01 && ++cThresholdMaintained >= 3)
            break;
    }

    result = Vector(NumberOfParams(), 1.0) + 2.0 * result;
    fAutoCorrelationTime = result;

    return fAutoCorrelationTime.get();
}

double ChainStats::GetAccRate()
{
    if (fAccRate)
        return fAccRate.get();

    auto itRange = GetIterators();

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

    fAccRate = (total == 0) ? 0.0 : (double) accepted / (double) total;

    return fAccRate.get();
}

void ChainSetStats::Reset()
{
    for (auto& chainStats : fSingleChainStats)
        chainStats.Reset();

    fRubinGelman.reset();
}

ChainStats& ChainSetStats::AddChain(const Chain& sampleChain)
{
    fSingleChainStats.emplace_back( sampleChain );
    fRubinGelman.reset();

    return fSingleChainStats.back();
}

void ChainSetStats::ClearChains()
{
    fSingleChainStats.clear();
    Reset();
}

void ChainSetStats::SelectRange(ptrdiff_t startIndex, ptrdiff_t endIndex)
{
    for (auto& chainStats : fSingleChainStats)
        chainStats.SelectRange( startIndex, endIndex );
    Reset();
}

void ChainSetStats::SelectPercentageRange(double start, double end)
{
    for (auto& chainStats : fSingleChainStats)
        chainStats.SelectPercentageRange( start, end );
    Reset();
}

double ChainSetStats::GetRubinGelman()
{
    if (fRubinGelman)
        return fRubinGelman.get();

    const size_t nChains = fSingleChainStats.size();

    if (nChains < 2)
        return 0.0;

    auto indexRange = fSingleChainStats.front().GetIndices();
    const size_t N = indexRange.second - indexRange.first;

    size_t nGens = fSingleChainStats[0].GetChain().size();
    for (size_t i = 1; i < nChains; i++) {
        const size_t icSize = fSingleChainStats[i].GetChain().size();
        nGens = min( nGens, icSize );
    }

    if (nGens < 10)
        return 0.0;

    const size_t d = fSingleChainStats[0].NumberOfParams();

    vector<Sample> chainsMeans( nChains, Sample(d) );
    vector<Sample> chainsVariances( nChains, Sample(d) );
    Sample globalMean(d);

    // for each chain
    for (size_t i = 0; i < nChains; ++i) {

        chainsMeans[i] = fSingleChainStats[i].GetMean();
        chainsVariances[i] = fSingleChainStats[i].GetVariance();

        globalMean += chainsMeans[i];
    }

    globalMean /= (double) nChains;

    Vector R (d, 0.0);

    // for each dimension
    for (uint32_t j = 0; j < d; ++j) {

        // variance between chains:
        double B = 0.0;
        // variance within chains:
        double W = 0.0;

        for (uint32_t i = 0; i < nChains; ++i) {
            B += math::pow<2>( chainsMeans[i].Values()[j] - globalMean.Values()[j] );
            W += chainsVariances[i].Values()[j];
        }
        B /= (double) (nChains - 1);
        W /= (double) nChains;

        const double V = ((double) (N - 1) / (double) N) * W + B + (B / (double) nChains);
        R[j] = V / W;
    }

    fRubinGelman = ublas::norm_inf(R);
    return fRubinGelman.get();
}

} /* namespace vmcmc */
