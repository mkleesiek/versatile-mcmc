/*
 * exceptions.h
 *
 *  Created on: 25.07.2016
 *      Author: marco.kleesiek@kit.edu
 */

#ifndef INCLUDE_EXCEPTIONS_H_
#define INCLUDE_EXCEPTIONS_H_

#include <exception>
#include <sstream>
#include <string>

namespace fmcmc
{

/**
 * Base class for exceptions thrown by FastMCMC.
 * The streaming operators allow easy in-place construction of exception messages.
 */
class Exception : public std::exception
{
public:
    const char* what() const throw() override;

    template<class XValue>
    Exception& operator<<(const XValue& toAppend);

    Exception& Nest(const std::exception& toNest);

protected:
    std::ostringstream fMessage;
    std::string fNestedMessage;

    mutable std::string fWhat;
};

inline const char* Exception::what() const
{
    fWhat = fMessage.str();
    if( !fNestedMessage.empty() ) {
        fWhat.append( " [" + fNestedMessage + "]" );
    }
    return fWhat.c_str();
}

template<class XValue>
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

}

#endif /* INCLUDE_EXCEPTIONS_H_ */
