/*
 * parameter.h
 *
 *  Created on: 26.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef SRC_FMCMC_PARAMETER_H_
#define SRC_FMCMC_PARAMETER_H_

#include <string>
#include <boost/optional.hpp>

namespace fmcmc
{

class Parameter
{
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

    double Constrain2Limits(double someValue) const;
    bool IsInsideLimits(double someValue) const;

protected:
    void CheckLimits();

    std::string fName;
    double fStartValue;
    double fAbsoluteErrorHint;
    boost::optional<double> fLowerLimit;
    boost::optional<double> fUpperLimit;
    bool fFixed;
};

} /* namespace fmcmc */

#endif /* SRC_FMCMC_PARAMETER_H_ */
