/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
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
