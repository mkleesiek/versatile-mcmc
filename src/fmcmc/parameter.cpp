/*
 * parameter.cpp
 *
 *  Created on: 26.07.2016
 *      Author: marco@kleesiek.com
 */

#include <fmcmc/parameter.h>
#include <fmcmc/exception.h>
#include <fmcmc/numeric.h>

using namespace std;
using namespace boost;

namespace fmcmc
{

Parameter Parameter::FixedParameter(const string& name, double startValue)
{
    return Parameter(name, startValue, 0.0, none, none, true);
}

Parameter::Parameter(const string& name, double startValue, double errorHint,
        optional<double> lowerLimit, optional<double> upperLimit, bool fixed) :
    fName( name ),
    fStartValue( startValue ),
    fAbsoluteErrorHint( errorHint ),
    fLowerLimit( lowerLimit ),
    fUpperLimit( upperLimit ),
    fFixed( fixed )
{
    CheckLimits();
}

Parameter::~Parameter()
{ }

void Parameter::SetRelativeErrorHint(double relError)
{
    fAbsoluteErrorHint = relError * fStartValue;
}

void Parameter::CheckLimits()
{
    if (fLowerLimit && !std::isfinite(fLowerLimit.get()))
        fLowerLimit = boost::none;
    if (fUpperLimit && !std::isfinite(fUpperLimit.get()))
        fUpperLimit = boost::none;

    if (fLowerLimit && fUpperLimit && fLowerLimit.get() > fUpperLimit.get()) {
        throw Exception() << "Fit parameter '" << fName << "' has a higher LowerLimit ("
            << fLowerLimit.get() << ") than its UpperLimit (" << fUpperLimit.get() << ").";
    }

    if (!IsInsideLimits(fStartValue)) {
        throw Exception() << "Start value (" << fStartValue << ") of fit parameter '"
            << fName << "' is not inside its specified limits ["
            << fLowerLimit.get_value_or( NaN<double>() ) << ", "
            << fUpperLimit.get_value_or( NaN<double>() ) << "].";
    }
}

bool Parameter::IsInsideLimits(double someValue) const
{
    return (!fLowerLimit || someValue >= fLowerLimit.get()) &&
           (!fUpperLimit || someValue <= fUpperLimit.get());
}

bool Parameter::ConstrainToLimits(double& someValue) const
{
    if (fLowerLimit && someValue < fLowerLimit.get()) {
        someValue = fLowerLimit.get();
        return true;
    }
    else if (fUpperLimit && someValue > fUpperLimit.get()) {
        someValue = fUpperLimit.get();
        return true;
    }
    else {
        return false;
    }
}

bool Parameter::ReflectFromLimits(double& someValue) const
{
    if (fLowerLimit && someValue < fLowerLimit.get()) {
        someValue = 2.0 * fLowerLimit.get() - someValue;
        // check if we've hit the other limit
        return !(fUpperLimit && someValue > fUpperLimit.get());

    }
    else if (fUpperLimit && someValue > fUpperLimit.get()) {
        someValue = 2.0 * fUpperLimit.get() - someValue;
        // check if we've hit the other limit
        return !(fLowerLimit && someValue < fLowerLimit.get());

    }
    else {
        return true;
    }
}

void ParameterSet::SetParameter(size_t pIndex, const Parameter& param)
{
    if (fParameters.size() <= pIndex)
        fParameters.resize( pIndex+1, Parameter::FixedParameter("", 0.0) );

    fParameters[pIndex] = param;
}

void ParameterSet::SetCorrelation(size_t p1, size_t p2, double correlation)
{
    if (p1 < p2)
        swap(p1, p2);
    else if (p1 == p2)
        return;

    // ensure minimum size
    const size_t minSize = max(p1, p2) + 1;
    if (fCorrelations.size1() <= minSize || fCorrelations.size2() <= minSize) {
        fCorrelations.resize(minSize, minSize, true);
    }

    limit(correlation, -1.0, 1.0);

    fCorrelations(p1, p2) = correlation;
}

ublas::vector<double> ParameterSet::GetStartValues() const
{
    ublas::vector<double> result( size() );
    for (size_t pIndex = 0; pIndex < size(); pIndex++) {
        result[pIndex] = GetParameter(pIndex).GetStartValue();
    }
    return result;
}

} /* namespace fmcmc */
