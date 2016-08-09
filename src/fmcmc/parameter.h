/*
 * parameter.h
 *
 *  Created on: 26.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef FMCMC_PARAMETER_H_
#define FMCMC_PARAMETER_H_

#include <fmcmc/ublas.h>

#include <string>
#include <boost/optional.hpp>

namespace fmcmc
{

class Parameter
{
public:
    static Parameter FixedParameter(const std::string& name = "", double startValue = 0.0);

public:
    Parameter(const std::string& name, double startValue, double absoluteErrorHint,
            boost::optional<double> lowerLimit = boost::none,
            boost::optional<double> upperLimit = boost::none,
            bool fixed = false);
    virtual ~Parameter();

    const std::string& GetName() const { return fName; }
    void SetName(const std::string& name) { fName = name; }

    double GetStartValue() const { return fStartValue; }
    void SetStartValue(double startValue) { fStartValue = startValue; }

    double GetAbsoluteErrorHint() const { return fAbsoluteErrorHint; }
    void SetAbsoluteErrorHint(double absoluteError) { fAbsoluteErrorHint = absoluteError; }

    void SetRelativeErrorHint(double relativeError);

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
    double fAbsoluteErrorHint;
    boost::optional<double> fLowerLimit;
    boost::optional<double> fUpperLimit;
    bool fFixed;
};

class ParameterSet
{
public:
    void SetParameter(size_t pIndex, const Parameter& param);
    const Parameter& GetParameter(size_t pIndex) const { return fParameters[pIndex]; }

    template<class XMatrixT>
    void SetCorrelationMatrix(const XMatrixT& matrix) { fCorrelations = matrix; }
    void SetCorrelation(size_t p1, size_t p2, double correlation);

    size_t size() const { return fParameters.size(); }
    Parameter& operator[](size_t pIndex) { return fParameters[pIndex]; }

    ublas::vector<double> GetStartValues() const;

private:
    std::vector<Parameter> fParameters;
    ublas::triangular_matrix<double, ublas::unit_lower> fCorrelations;
};

} /* namespace fmcmc */

#endif /* FMCMC_PARAMETER_H_ */
