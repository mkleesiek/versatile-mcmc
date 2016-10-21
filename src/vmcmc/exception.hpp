/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 25.07.2016
 * @author marco@kleesiek.com
 *
 * @brief VMCMC custom exception classes.
 */

#ifndef VMCMC_EXCEPTION_H_
#define VMCMC_EXCEPTION_H_

#include <exception>
#include <sstream>
#include <string>

namespace vmcmc
{

/**
 * Base class for exceptions thrown by FastMCMC.
 *
 * The streaming operators allow easy in-place construction of exception messages.
 */
class Exception : public std::exception
{
public:
    Exception() = default;
    Exception(const Exception& copy);
    virtual ~Exception() = default;

    void operator=(const Exception& copy);

    const char* what() const throw() override;

    template <typename XValue>
    Exception& operator<<(const XValue& toAppend);

    Exception& Nest(const std::exception& toNest);

protected:
    std::ostringstream fMessage;
    std::string fNestedMessage;

    mutable std::string fWhat;
};

inline Exception::Exception( const Exception& toCopy ) :
    std::exception( toCopy ),
    fMessage( toCopy.fMessage.str() ),
    fNestedMessage( toCopy.fNestedMessage )
{ }

inline void Exception::operator=( const Exception& toCopy )
{
    std::exception::operator=( toCopy );
    fMessage.str( toCopy.fMessage.str() );
    fNestedMessage = toCopy.fNestedMessage;
}

inline const char* Exception::what() const throw()
{
    fWhat = fMessage.str();
    if( !fNestedMessage.empty() ) {
        fWhat.append( " [" + fNestedMessage + "]" );
    }
    return fWhat.c_str();
}

template<typename XValue>
inline Exception& Exception::operator<<(const XValue& toAppend)
{
    fMessage << toAppend;
    return (*this);
}

inline Exception& Exception::Nest(const std::exception& toNest)
{
    fNestedMessage = toNest.what();
    return (*this);
}

inline std::ostream& operator<<(std::ostream& os, const Exception& e)
{
    return os << e.what();
}

} /* namespace vmcmc */

#endif /* VMCMC_EXCEPTION_H_ */
