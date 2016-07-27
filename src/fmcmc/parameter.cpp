/*
 * parameter.cpp
 *
 *  Created on: 26.07.2016
 *      Author: marco@kleesiek.com
 */

#include "fmcmc/parameter.h"
#include "fmcmc/exception.h"
#include "fmcmc/numeric.h"

using namespace std;
using namespace boost;

namespace fmcmc
{

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

double Parameter::Constrain2Limits(double someValue) const
{
    if (fLowerLimit && someValue < fLowerLimit.get())
        return fLowerLimit.get();
    else if (fUpperLimit && someValue > fUpperLimit.get())
        return fUpperLimit.get();
    else
        return someValue;
}

} /* namespace fmcmc */
