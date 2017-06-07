#pragma once

#include <experimental/numeric>
#include <numeric>
#include <ratio>

#include <iostream>

namespace si
{
namespace detail
{
template <int _m = 0, int _kg = 0, int _s = 0, int _A = 0, int _K = 0, int _mol = 0, int _cd = 0>
struct base {
    constexpr static int m   = _m;
    constexpr static int kg  = _kg;
    constexpr static int s   = _s;
    constexpr static int A   = _A;
    constexpr static int K   = _K;
    constexpr static int mol = _mol;
    constexpr static int cd  = _cd;
};

// base units                         m kg  s  A  K mol cd
template<int p = 1> using _m   = base<p, 0, 0, 0, 0, 0, 0>;
template<int p = 1> using _kg  = base<0, p, 0, 0, 0, 0, 0>;
template<int p = 1> using _s   = base<0, 0, p, 0, 0, 0, 0>;
template<int p = 1> using _A   = base<0, 0, 0, p, 0, 0, 0>;
template<int p = 1> using _K   = base<0, 0, 0, 0, p, 0, 0>;
template<int p = 1> using _mol = base<0, 0, 0, 0, 0, p, 0>;
template<int p = 1> using _cd  = base<0, 0, 0, 0, 0, 0, p>;

// derived units                      m kg  s  A  K mol cd
template<int p = 1> using _Hz  = base< 0, 0,   -p, 0, 0, 0, 0>;
template<int p = 1> using _N   = base< p, p, -2*p, 0, 0, 0, 0>;
template<int p = 1> using _Pa  = base<-p, p, -2*p, 0, 0, 0, 0>;

// radian and steradian are dimensionless, unfortunately that means that they cannot be
// differentiated with the current implementation.
template<int p = 1> using _rad = base<>;
template<int p = 1> using _sr  = base<>;

template <typename Lhs, typename Rhs>
using base_multiply = base<Lhs::m + Rhs::m,
                           Lhs::kg + Rhs::kg,
                           Lhs::s + Rhs::s,
                           Lhs::A + Rhs::A,
                           Lhs::K + Rhs::K,
                           Lhs::mol + Rhs::mol,
                           Lhs::cd + Rhs::cd>;

template <typename Lhs, typename Rhs>
using base_divide = base<Lhs::m - Rhs::m,
                         Lhs::kg - Rhs::kg,
                         Lhs::s - Rhs::s,
                         Lhs::A - Rhs::A,
                         Lhs::K - Rhs::K,
                         Lhs::mol - Rhs::mol,
                         Lhs::cd - Rhs::cd>;
} // namespace detail

template<typename _Rep, typename _Ratio, typename _Base>
struct unit;

template<typename _ToUnit, typename _Rep, typename _Ratio, typename _Base>
auto unit_cast(const unit<_Rep, _Ratio, _Base> &other)
    -> std::enable_if_t<std::is_same<typename _ToUnit::base, _Base>::value, _ToUnit>
{
    using to_ratio   = typename _ToUnit::ratio;
    using to_rep     = typename _ToUnit::rep;
    using ratio_tf   = std::ratio_divide<_Ratio, to_ratio>;
    using common_rep = typename std::common_type_t<_Rep, to_rep>;

    auto v = static_cast<common_rep>(other.count()
            * static_cast<common_rep>(ratio_tf::num)
            / static_cast<common_rep>(ratio_tf::den));
    return _ToUnit{v};
}

template<typename _Rep, typename _Ratio, typename _Base>
struct unit
{
    using rep   = _Rep;
    using ratio = _Ratio;
    using base  = _Base;

    constexpr unit() = default;
    explicit constexpr unit(rep count)
        : _count(count) { }

    template<typename _Rep2, typename _Ratio2, typename _Base2>
    constexpr unit(const unit<_Rep2, _Ratio2, _Base2> &other)
        : _count(unit_cast<unit>(other).count()) { }

    constexpr rep count() const { return _count; }

private:
    rep _count = 0;
}; // class unit

template<typename _Rep, typename _Ratio = std::ratio<1>>
using length = unit<_Rep, _Ratio, detail::_m<1>>;

template<typename _Rep, typename _Ratio = std::ratio<1>>
using mass = unit<_Rep, _Ratio, detail::_kg<1>>;

template<typename _Rep, typename _Ratio = std::ratio<1>>
using time = unit<_Rep, _Ratio, detail::_s<1>>;

template<typename _Rep, typename _Ratio = std::ratio<1>>
using current = unit<_Rep, _Ratio, detail::_A<1>>;

template<typename _Rep, typename _Ratio = std::ratio<1>>
using temperature = unit<_Rep, _Ratio, detail::_K<1>>;

template<typename _Rep, typename _Ratio = std::ratio<1>>
using amount = unit<_Rep, _Ratio, detail::_mol<1>>;

template<typename _Rep, typename _Ratio = std::ratio<1>>
using luminous_intensity = unit<_Rep, _Ratio, detail::_cd<1>>;
} // namespace si

namespace std
{
template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2>
struct common_type<si::unit<_Rep1, _Ratio1, _Base1>,
                   si::unit<_Rep2, _Ratio2, _Base2>> {
private:
    static constexpr auto gcd_num = std::experimental::gcd(_Ratio1::num, _Ratio2::num);
    static constexpr auto gcd_den = std::experimental::gcd(_Ratio1::den, _Ratio2::den);

    using common_ratio = std::ratio<gcd_num, (_Ratio1::den / gcd_den) * _Ratio2::den>;
    using common_rep   = std::common_type_t<_Rep1, _Rep2>;
    using common_base  = typename std::enable_if_t<std::is_same<_Base1, _Base2>::value, _Base1>;
public:
    using type = si::unit<common_rep, common_ratio, common_base>;
};
} // namespace std

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator==(const si::unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const si::unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using u1 = si::unit<_Rep1, _Ratio1, _Base1>;
    using u2 = si::unit<_Rep2, _Ratio2, _Base2>;
    using common_unit = typename std::common_type_t<u1, u2>;

    return common_unit{lhs}.count() == common_unit{rhs}.count();
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator<(const si::unit<_Rep1, _Ratio1, _Base1> &lhs,
        const si::unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using u1 = si::unit<_Rep1, _Ratio1, _Base1>;
    using u2 = si::unit<_Rep2, _Ratio2, _Base2>;
    using common_unit = typename std::common_type_t<u1, u2>;

    return common_unit{lhs}.count() < common_unit{rhs}.count();
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator!=(const si::unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const si::unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    return !(lhs == rhs);
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator<=(const si::unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const si::unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    return !(rhs < lhs);
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator>(const si::unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const si::unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    return rhs < lhs;
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator>=(const si::unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const si::unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    return !(lhs < rhs);
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          typename = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator+(const si::unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const si::unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using u1 = si::unit<_Rep1, _Ratio1, _Base1>;
    using u2 = si::unit<_Rep2, _Ratio2, _Base2>;
    using common_unit = typename std::common_type_t<u1, u2>;

    return common_unit{common_unit{lhs}.count() + common_unit{rhs}.count()};
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          std::enable_if_t<std::is_same<_Base1, _Base2>::value, int> = 0>
constexpr auto operator-(const si::unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const si::unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using u1 = si::unit<_Rep1, _Ratio1, _Base1>;
    using u2 = si::unit<_Rep2, _Ratio2, _Base2>;
    using common_unit = typename std::common_type_t<u1, u2>;

    return common_unit{common_unit{lhs}.count() - common_unit{rhs}.count()};
}
