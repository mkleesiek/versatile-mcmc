/**
 * @file
 *
 * @copyright Copyright 2016 Marco Kleesiek.
 * Released under the GNU Lesser General Public License v3.
 *
 * @date 12.08.2016
 * @author marco@kleesiek.com
 *
 * @brief Typetrait utility structs.
 */

#ifndef VMCMC_TYPETRAITS_H_
#define VMCMC_TYPETRAITS_H_

#include <type_traits>
#include <string>
#include <vector>

namespace vmcmc {

/**
 * Typetrait checking for STL like container structure.
 *
 * This typetrait checks whether the passed template argument is an STL-like
 * container. In that case, the static ::value member variable evaluates
 * to true.
 * Inspired by http://stackoverflow.com/a/16316640/6908762.
 */
template<typename T>
struct is_container
{
    using test_type = typename std::remove_const<T>::type;

    template<typename A>
    static constexpr bool test(
        A* pt,
        const A* cpt                    = nullptr,
        decltype( pt->begin()  )*       = nullptr,
        decltype( pt->end()    )*       = nullptr,
        decltype( cpt->begin() )*       = nullptr,
        decltype( cpt->end()   )*       = nullptr,

//        decltype( pt->clear()  )*        =   nullptr,
        decltype( pt->size()   )*       = nullptr,

        typename A::iterator* pi        = nullptr,
        typename A::const_iterator* pci = nullptr,
        typename A::value_type* /*pv*/  = nullptr )
    {
        using iterator                  = typename A::iterator;
        using const_iterator            = typename A::const_iterator;
        using value_type                = typename A::value_type;

        return
            ( std::is_same<decltype(pt->begin()),iterator>::value || std::is_same<decltype(pt->begin()),const_iterator>::value) &&
            ( std::is_same<decltype(pt->end()),iterator>::value   || std::is_same<decltype(pt->end()),const_iterator>::value  ) &&
              std::is_same<decltype(cpt->begin()),const_iterator>::value &&
              std::is_same<decltype(cpt->end()),const_iterator>::value &&
            ( std::is_same<decltype(**pi),value_type &>::value    || std::is_same<decltype(**pi),value_type const &>::value   ) &&
              std::is_same<decltype(**pci),value_type const &>::value;
    }

    template<typename A>
    static constexpr bool test(...)
    {
        return false;
    }

    static constexpr bool value = test<test_type>(nullptr);
};

/// @cond
///
template <>
struct is_container<std::vector<bool>>
{
    static constexpr bool value = true;
};

template <>
struct is_container<std::string>
{
    static constexpr bool value = false;
};
/// @endcond

}

#endif /* VMCMC_TYPETRAITS_H_ */
