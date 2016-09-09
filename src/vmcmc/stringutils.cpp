/**
 * @file stringutils.cpp
 *
 * @date 17.08.2016
 * @author marco@kleesiek.com
 *
 */

#include <vmcmc/stringutils.hpp>
#include <vmcmc/sample.hpp>

using namespace std;

namespace vmcmc {

ostream& operator<< (ostream& strm, const Sample& sample)
{
    strm << sample.Values() << " " << sample.GetPrior()
         << " (" << sample.GetLikelihood() << ", " << sample.GetNegLogLikelihood() << ")";
    return strm;
}

}
