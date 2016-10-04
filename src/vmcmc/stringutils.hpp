/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 29.07.2016
 * @author marco@kleesiek.com
 *
 * @brief String utility functions.
 */

#ifndef VMCMC_STRINGUTILS_H_
#define VMCMC_STRINGUTILS_H_

#include <vmcmc/typetraits.hpp>

#include <sstream>
#include <string>
#include <utility>

namespace vmcmc
{

class Sample;

/**
 * Join an STL style container and output to a stream with its values joined by
 * a separator.
 * @param stream The output stream.
 * @param sequence The container to be serialized. Must provide STL style
 * iterators.
 * @param separator The separator to be inserted between printed values.
 * @return Reference to the output stream.
 */
template <typename SequenceT, typename SeparatorT>
inline std::ostream& join(std::ostream& stream, const SequenceT& sequence, const SeparatorT& separator = "")
{
    auto itBegin = std::begin(sequence);
    auto itEnd = std::end(sequence);

    if (itBegin != itEnd) {
        stream << *itBegin;
        ++itBegin;
    }

    for (; itBegin != itEnd; ++itBegin) {
        stream << separator << *itBegin;
    }

    return stream;
}

/**
 * Join an STL style container to a string.
 * @param sequence The container to be serialized. Must provide STL style
 * iterators.
 * @param separator The separator to be inserted between serialized values.
 * @return The joined string.
 */
template <typename SequenceT, typename SeparatorT>
inline std::string join(const SequenceT& sequence, const SeparatorT& separator, int floatPrecision = -1)
{
    std::ostringstream strm;
    if (floatPrecision > -1)
        strm.precision(floatPrecision);
    join(strm, sequence, separator);
    return strm.str();
}

/**
 * Custom stream operator overload for Sample.
 * @param strm
 * @param sample
 * @return
 */
std::ostream& operator<< (std::ostream& strm, const Sample& sample);

/**
 * Serialize any STL pair to an output stream.
 * @param strm An output stream.
 * @param pair A pair.
 * @return Reference to the output stream.
 */
template <typename T1, typename T2>
inline std::ostream& operator<< (std::ostream& strm, const std::pair<T1, T2>& pair)
{
    strm << "(" << pair.first << ", " << pair.second << ")";
    return strm;
}

/**
 * Serialize any STL container to an output stream.
 * @param strm An output stream.
 * @param container A container fulfilling the type trait #vmcmc::is_container.
 * @return Reference to the output stream.
 */
template <typename ContainerT, class = typename std::enable_if<vmcmc::is_container<ContainerT>::value>::type>
inline std::ostream& operator<< (std::ostream& strm, const ContainerT& container)
{
    strm << "[" << container.size() << "](";

    auto itBegin = std::begin(container);
    auto itEnd = std::end(container);

    if (itBegin != itEnd) {
        strm << *itBegin;
        itBegin++;
    }

    for (; itBegin != itEnd; ++itBegin) {
        strm << ", " << *itBegin;
    }

    strm << ")";

    return strm;
}

}

#endif /* VMCMC_STRINGUTILS_H_ */
