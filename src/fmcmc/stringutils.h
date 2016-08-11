/*
 * stringutils.h
 *
 *  Created on: 29.07.2016
 *      Author: marco@kleesiek.com
 */

#ifndef FMCMC_STRINGUTILS_H_
#define FMCMC_STRINGUTILS_H_

#include <iterator>
#include <iostream>
#include <string>

//#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/matrix.hpp>
//#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace fmcmc {

template <class SequenceT, class SeparatorT>
inline std::ostream& join(std::ostream& stream, const SequenceT& sequence, const SeparatorT& separator)
{
    auto itBegin = std::begin(sequence);
    auto itEnd = std::end(sequence);

    // Append first element
    if (itBegin != itEnd) {
        stream << *itBegin;
        ++itBegin;
    }

    for (; itBegin != itEnd; ++itBegin) {
        stream << separator;
        stream << *itBegin;
    }

    return stream;
}

template <class SequenceT, class SeparatorT>
inline std::string join(const SequenceT& sequence, const SeparatorT& separator)
{
    std::ostringstream strm;
    join(strm, sequence, separator);
    return strm.str();
}

//template <class T>
//inline std::ostream& operator<< (std::ostream& os, const boost::numeric::ublas::vector<T>& vector)
//{
//    typedef typename boost::numeric::ublas::matrix<T>::size_type size_type;
//
//    os << "[" << vector.size() << "]";
//    os << " (";
//    if (vector.size() > 0)
//        os << vector(0);
//    for (size_type c = 1; c < vector.size(); ++c)
//        os << ", " << vector(c);
//    os << ")";
//    return os;
//}
//
//template <class T>
//inline std::ostream& operator<< (std::ostream& os, const boost::numeric::ublas::matrix<T>& matrix)
//{
//    typedef typename boost::numeric::ublas::matrix<T>::size_type size_type;
//
//    os << "[" << matrix.size1() << "," << matrix.size2() << "]";
//
//    for (size_type r = 0; r < matrix.size1(); ++r) {
//        os << std::endl << "(";
//        for (size_type c = 0; c < matrix.size2(); ++c) {
//            os.width(os.precision() + 7);
//            os << matrix(r, c);
//        }
//        os << " )";
//    }
//    return os;
//}
//
//template <class T>
//inline std::ostream& operator<< (std::ostream& os, const boost::numeric::ublas::triangular_matrix<T>& matrix)
//{
//    typedef typename boost::numeric::ublas::triangular_matrix<T>::size_type size_type;
//
//    os << "[" << matrix.size1() << "," << matrix.size2() << "]";
//
//    for (size_type r = 0; r < matrix.size1(); ++r) {
//        os << std::endl << "(";
//        for (size_type c = 0; c < matrix.size2(); ++c) {
//            os.width(os.precision() + 7);
//            os << matrix(r, c);
//        }
//        os << " )";
//    }
//    return os;
//}

}



#endif /* FMCMC_STRINGUTILS_H_ */
