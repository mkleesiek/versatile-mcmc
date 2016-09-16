/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 05.08.2016
 * @author marco@kleesiek.com
 *
 * @brief Contains the Sample class definition for data sampled from the target
 * parameter space.
 */

#ifndef VMCMC_SAMPLE_H_
#define VMCMC_SAMPLE_H_

#include <vmcmc/blas.hpp>
#include <vmcmc/numeric.hpp>

#include <initializer_list>

namespace vmcmc
{

/**
 * A 'sample' represents a node or data point in a Markov Chain.
 * It holds information about the generation, when it was sampled, the point in
 * the parameter space and the corresponding values for the likelihood,
 * -log(likelihood) and the prior value.
 */
class Sample
{
public:
    Sample(const Vector& pValues);
    Sample(std::initializer_list<double> pValues);
    Sample(size_t nParams = 0);
    virtual ~Sample() { }

    void SetGeneration(size_t value) { fGeneration = value; }
    size_t GetGeneration() const { return fGeneration; }
    size_t IncrementGeneration() { return ++fGeneration; }

    Vector& Values() { return fParameterValues; }
    const Vector& Values() const { return fParameterValues; }
    Sample& operator= (std::initializer_list<double> pValues);

    /**
     * Reset ::fLikelihood, ::fNegLogLikelihood and ::fPrior to their
     * default values.
     */
    void Reset();

    void SetLikelihood(double value) { fLikelihood = value; }
    double GetLikelihood() const { return fLikelihood; }

    void SetNegLogLikelihood(double value) { fNegLogLikelihood = value; }
    double GetNegLogLikelihood() const { return fNegLogLikelihood; }

    void SetPrior(double value) { fPrior = value; }
    double GetPrior() const { return fPrior; }

    void SetAccepted(bool value) { fAccepted = value; }
    double IsAccepted() const { return fAccepted; }

    void operator+= (const Sample& other);
    void operator-= (const Sample& other);
    void operator*= (double factor);
    void operator/= (double factor);

    friend Sample operator+(const Sample& s1, const Sample& s2);
    friend Sample operator-(const Sample& s1, const Sample& s2);
    friend Sample operator*(const Sample& s1, double f);
    friend Sample operator/(const Sample& s1, double f);

private:
    size_t fGeneration       = 0;
    Vector fParameterValues;
    double fLikelihood       = 0.0;
    double fNegLogLikelihood = -numeric::inf();
    double fPrior            = 0.0;
    bool fAccepted           = false;
};

inline Sample::Sample(const Vector& pValues) :
    fParameterValues( pValues )
{ }

inline Sample::Sample(std::initializer_list<double> pValues) :
    fParameterValues( pValues )
{ }

inline Sample::Sample(size_t nParams) :
    fParameterValues( nParams, 0.0 )
{ }

inline Sample& Sample::operator= (std::initializer_list<double> pValues)
{
    fParameterValues = Vector(pValues);
    return *this;
}

inline void Sample::Reset()
{
    fPrior = fLikelihood = 0.0;
    fNegLogLikelihood = -numeric::inf();
    fAccepted = false;
}

// unary operators

inline void Sample::operator+= (const Sample& other)
{
    fParameterValues += other.fParameterValues;
    Reset();
}

inline void Sample::operator-= (const Sample& other)
{
    fParameterValues -= other.fParameterValues;
    Reset();
}

inline void Sample::operator*= (double factor)
{
    fParameterValues *= factor;
    Reset();
}

inline void Sample::operator/= (double factor)
{
    fParameterValues /= factor;
    Reset();
}

// free binary operators

inline Sample operator+(const Sample& s1, const Sample& s2)
{
    return Sample( s1.fParameterValues + s2.fParameterValues );
}

inline Sample operator-(const Sample& s1, const Sample& s2)
{
    return Sample( s1.fParameterValues - s2.fParameterValues );
}

inline Sample operator*(const Sample& s1, double f)
{
    return Sample( s1.fParameterValues * f );
}

inline Sample operator/(const Sample& s1, double f)
{
    return Sample( s1.fParameterValues / f );
}

} /* namespace vmcmc */

#endif /* VMCMC_SAMPLE_H_ */
