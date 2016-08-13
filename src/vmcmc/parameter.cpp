/**
 * @file
 *
 * @date 26.07.2016
 * @author marco@kleesiek.com
 */

#include <vmcmc/parameter.h>
#include <vmcmc/exception.h>
#include <vmcmc/numeric.h>
#include <vmcmc/stringutils.h>
#include <vmcmc/logger.h>

using namespace std;
using namespace boost;

namespace vmcmc
{

LOG_DEFINE("vmcmc.parameter");

Parameter Parameter::FixedParameter(const string& name, double startValue)
{
    return Parameter(name, startValue, 0.0, none, none, true);
}

Parameter::Parameter(const string& name, double startValue, double errorHint,
        optional<double> lowerLimit, optional<double> upperLimit, bool fixed) :
    fName( name ),
    fStartValue( startValue ),
    fAbsoluteError( errorHint ),
    fLowerLimit( lowerLimit ),
    fUpperLimit( upperLimit ),
    fFixed( fixed )
{
    CheckLimits();
}

Parameter::~Parameter()
{ }

void Parameter::SetRelativeError(double relError)
{
    fAbsoluteError = relError * fStartValue;
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
    const size_t currentSize = fCorrelations.size1();
    const size_t minSize = max(p1, p2) + 1;

    if (currentSize < minSize) {
        fCorrelations.resize(minSize, minSize, true);
        for (size_t i = currentSize; i < minSize; i++)
            for (size_t j = 0; j < i; j++)
                fCorrelations(i, j) = 0.0;
    }

    limit(correlation, -1.0, 1.0);

    fCorrelations(p1, p2) = correlation;
}

double ParameterSet::GetCorrelation(size_t p1, size_t p2) const
{
    if (p1 < p2)
        swap(p1, p2);
    else if (p1 == p2)
        return 1.0;

    return fCorrelations(p1, p2);
}

Vector ParameterSet::GetStartValues() const
{
    Vector result( size() );
    for (size_t pIndex = 0; pIndex < size(); pIndex++) {
        result[pIndex] = GetParameter(pIndex).GetStartValue();
    }
    return result;
}

MatrixLower ParameterSet::GetCovarianceMatrix() const
{
    MatrixLower result( size(), size() );

    for (size_t i = 0; i < size(); ++i) {
        for (size_t j = 0; j <= i; ++j) {
            result(i, j) = fCorrelations(i, j)
                * fParameters[i].GetAbsoluteError() * fParameters[j].GetAbsoluteError();
        }
    }

    return result;
}

MatrixLower ParameterSet::GetCholeskyDecomp() const
{
    const MatrixLower cov = GetCovarianceMatrix();

    MatrixLower result(cov.size1(), cov.size2());

    const size_t statusDecomp = choleskyDecompose(cov, result);
    if (statusDecomp != 0) {
//        throw Exception() << "Cholesky decomposition of covariance matrix " << cov << " failed.";
        LOG(Error, "Cholesky decomposition of covariance matrix " << cov << " failed.");

        for (size_t i = 0; i < result.size1(); i++) {
            for (size_t j = 0; j < i; j++)
                result(i, j) = 0.0;
            result(i, i) = fParameters[i].GetAbsoluteError();
        }
    }

    return result;
}

void ParameterSet::ScaleErrors(double scaling)
{
    for (auto& param : fParameters)
        param.SetAbsoluteError( scaling * param.GetAbsoluteError() );
}

} /* namespace vmcmc */
