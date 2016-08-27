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
#include <vmcmc/blas.h>

namespace vmcmc
{

/**
 * This class represents a fit parameter of the target function to be sampled
 * from. Its purpose is to instruct the MCMC algorithm at what point
 * in the parameter space (start value) with what step size (error) to start
 * sampling.
 * It also holds optional lower and upper value limits, which correspond to
 * step-function like priors. If the property 'fixed' is set, the sampler should
 * never vary the parameter from its start value.
 *
 */
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

    /**
     * Sets the absolute error by multiplying a relative error argument
     * with the before-hand configured start value.
     * @param relativeError
     */
    void SetRelativeError(double relativeError);

    const boost::optional<double>& GetLowerLimit() const { return fLowerLimit; }
    void SetLowerLimit(const boost::optional<double>& lowerLimit) { fLowerLimit = lowerLimit; }

    const boost::optional<double>& GetUpperLimit() const { return fUpperLimit; }
    void SetUpperLimit(const boost::optional<double>& upperLimit) { fUpperLimit = upperLimit; }

    bool IsFixed() const { return fFixed; }
    void SetFixed(bool fixed) { fFixed = fixed; }

    /**
     * Checks if some parameter value is inside the configured parameter limits.
     * @param someValue
     * @return False, if @p someValue < Parameter::fLowerLimit || @p someValue > Parameter::fUpperLimit.
     */
    bool IsInsideLimits(double someValue) const;

    /**
     * If a value lies outside the parameter limits, constrain it accordingly.
     * @param someValue A reference to the value to be constrained.
     */
    void ConstrainToLimits(double& someValue) const;

    /**
     * If a value lies outside a parameter limits, attempt a reflection from
     * the closer limit.
     * @param someValue A reference to the value to be reflected.
     * @return True, if @p someValue did not have to be modified or was
     * reflected successfully (without exceeding the opposite limit).
     */
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

/**
 * A list of parameters, describing the parameter space of the target function
 * to be evaluated. In addition to listing the individual parameter properties,
 * a correlation matrix can be specified.
 */
class ParameterConfig
{
public:
    ParameterConfig();
    virtual ~ParameterConfig();

    void SetParameter(size_t pIndex, const Parameter& param);
//    const Parameter& GetParameter(size_t pIndex) const { return fParameters[pIndex]; }

    size_t size() const { return fParameters.size(); }
    Parameter& operator[](size_t pIndex) { return fParameters[pIndex]; }
    const Parameter& operator[](size_t pIndex) const { return fParameters[pIndex]; }

    /**
     * Set an error scaling factor, which is applied to all parameter errors
     * uniformly.
     * @param scaling
     */
    void SetErrorScaling(double scaling) { fErrorScaling = scaling; }
    double GetErrorScaling() const { return fErrorScaling; }

    Vector GetStartValues(bool randomized = false) const;
    Vector GetErrors() const;

    /**
     * Set a correlation matrix. This should be a lower unit triangular matrix
     * with size corresponding to the number of parameters.
     * @param matrix
     */
    template<class XMatrixT>
    void SetCorrelationMatrix(const XMatrixT& matrix) { fCorrelations = matrix; }
    const MatrixUnitLower& GetCorrelationMatrix() const { return fCorrelations; }

    void SetCorrelation(size_t p1, size_t p2, double correlation);
    double GetCorrelation(size_t p1, size_t p2) const;

    MatrixLower GetCovarianceMatrix() const;
    MatrixLower GetCholeskyDecomp() const;

    /**
     * Checks if some vector is inside the configured parameter limits.
     * @param somePoint
     * @return False, if for some parameter index i,
     * @p somePoint[i] < fParameter[i].fLowerLimit || @p somePoint[i] > fParameter[i].fUpperLimit.
     */
    bool IsInsideLimits(const Vector& somePoint) const;

    /**
     * If a vector lies outside the parameter limits, constrain it accordingly.
     * @param somePoint A reference to the vector to be constrained.
     */
    void ConstrainToLimits(Vector& somePoint) const;

    /**
     * If a vector lies outside the parameter limits, attempt a reflection from
     * the closest limit in the affected dimension.
     * @param somePoint A reference to the vector to be reflected.
     * @return True, if @p somePoint did not have to be modified or was
     * reflected successfully (without exceeding any opposite limit).
     */
    bool ReflectFromLimits(Vector& somePoint) const;

private:
    std::vector<Parameter> fParameters;
    double fErrorScaling;
    MatrixUnitLower fCorrelations;
};

} /* namespace vmcmc */

#endif /* FMCMC_PARAMETER_H_ */
