/**
 * @file
 *
 * @date 26.07.2016
 * @author marco@kleesiek.com
 */

#ifndef FMCMC_PARAMETER_H_
#define FMCMC_PARAMETER_H_

#include <string>
#include <boost/optional.hpp>
#include "blas.h"

namespace vmcmc
{

class Parameter
{
public:
    static Parameter FixedParameter(const std::string& name = "", double startValue = 0.0);

public:
    Parameter(const std::string& name, double startValue, double absoluteError,
            boost::optional<double> lowerLimit = boost::none,
            boost::optional<double> upperLimit = boost::none,
            bool fixed = false);
    virtual ~Parameter();

    const std::string& GetName() const { return fName; }
    void SetName(const std::string& name) { fName = name; }

    double GetStartValue() const { return fStartValue; }
    void SetStartValue(double startValue) { fStartValue = startValue; }

    double GetAbsoluteError() const { return fAbsoluteError; }
    void SetAbsoluteError(double absoluteError) { fAbsoluteError = absoluteError; }

    void SetRelativeError(double relativeError);

    const boost::optional<double>& GetLowerLimit() const { return fLowerLimit; }
    void SetLowerLimit(const boost::optional<double>& lowerLimit) { fLowerLimit = lowerLimit; }

    const boost::optional<double>& GetUpperLimit() const { return fUpperLimit; }
    void SetUpperLimit(const boost::optional<double>& upperLimit) { fUpperLimit = upperLimit; }

    bool IsFixed() const { return fFixed; }
    void SetFixed(bool fixed) { fFixed = fixed; }

    bool IsInsideLimits(double someValue) const;
    bool ConstrainToLimits(double& someValue) const;
    bool ReflectFromLimits(double& someValue) const;

protected:
    void CheckLimits();

    std::string fName;
    double fStartValue;
    double fAbsoluteError;
    boost::optional<double> fLowerLimit;
    boost::optional<double> fUpperLimit;
    bool fFixed;
};

class ParameterSet
{
public:
    ParameterSet();
    virtual ~ParameterSet();

    void SetParameter(size_t pIndex, const Parameter& param);
    const Parameter& GetParameter(size_t pIndex) const { return fParameters[pIndex]; }

    size_t size() const { return fParameters.size(); }
    Parameter& operator[](size_t pIndex) { return fParameters[pIndex]; }
    const Parameter& operator[](size_t pIndex) const { return fParameters[pIndex]; }

    void SetErrorScaling(double scaling) { fErrorScaling = scaling; }
    double GetErrorScaling() const { return fErrorScaling; }

    Vector GetStartValues(bool randomized = false) const;
    Vector GetErrors() const;

    template<class XMatrixT>
    void SetCorrelationMatrix(const XMatrixT& matrix) { fCorrelations = matrix; }
    const MatrixUnitLower& GetCorrelationMatrix() const { return fCorrelations; }

    void SetCorrelation(size_t p1, size_t p2, double correlation);
    double GetCorrelation(size_t p1, size_t p2) const;

    MatrixLower GetCovarianceMatrix() const;
    MatrixLower GetCholeskyDecomp() const;

    bool IsInsideLimits(Vector somePoint) const;
    bool ConstrainToLimits(Vector& somePoint) const;
    bool ReflectFromLimits(Vector& somePoint) const;

private:
    std::vector<Parameter> fParameters;
    double fErrorScaling;
    MatrixUnitLower fCorrelations;
};

} /* namespace vmcmc */

#endif /* FMCMC_PARAMETER_H_ */
