#pragma once

#include <chrono>
#include <experimental/numeric>
#include <numeric>
#include <ratio>

#include <iostream>

namespace si
{
template<typename _Rep, typename _Ratio, typename _Base>
struct unit;
} // namespace si

namespace std
{
template <intmax_t _Num1, intmax_t _Den1,
          intmax_t _Num2, intmax_t _Den2>
struct common_type<std::ratio<_Num1, _Den1>, std::ratio<_Num2, _Den2>> {
private:
    static constexpr auto gcd_num = std::experimental::gcd(_Num1, _Num2);
    static constexpr auto gcd_den = std::experimental::gcd(_Den1, _Den2);

public:
    using type = std::ratio<gcd_num, (_Den1 / gcd_den) * _Den2>;
};

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2>
struct common_type<si::unit<_Rep1, _Ratio1, _Base1>,
                   si::unit<_Rep2, _Ratio2, _Base2>> {
private:
    using common_rep   = std::common_type_t<_Rep1, _Rep2>;
    using common_ratio = std::common_type_t<_Ratio1, _Ratio2>;
    using common_base  = typename std::enable_if_t<std::is_same<_Base1, _Base2>::value, _Base1>;
public:
    using type = si::unit<common_rep, common_ratio, common_base>;
};
} // namespace std

namespace si
{
namespace detail
{
template <int _m = 0, int _g = 0, int _s = 0, int _A = 0, int _K = 0, int _mol = 0, int _cd = 0>
struct base {
    constexpr static int m   = _m;
    constexpr static int g   = _g;
    constexpr static int s   = _s;
    constexpr static int A   = _A;
    constexpr static int K   = _K;
    constexpr static int mol = _mol;
    constexpr static int cd  = _cd;
};

// base units                         m  g  s  A  K mol cd
template<int p = 1> using _m   = base<p, 0, 0, 0, 0, 0, 0>;
template<int p = 1> using _g   = base<0, p, 0, 0, 0, 0, 0>;
template<int p = 1> using _s   = base<0, 0, p, 0, 0, 0, 0>;
template<int p = 1> using _A   = base<0, 0, 0, p, 0, 0, 0>;
template<int p = 1> using _K   = base<0, 0, 0, 0, p, 0, 0>;
template<int p = 1> using _mol = base<0, 0, 0, 0, 0, p, 0>;
template<int p = 1> using _cd  = base<0, 0, 0, 0, 0, 0, p>;

// radian and steradian are dimensionless, unfortunately that means that they cannot be
// differentiated with the current implementation.
template<int p = 1> using _rad = base<>;
template<int p = 1> using _sr  = base<>;

template <typename Lhs, typename Rhs>
using base_multiply = base<Lhs::m + Rhs::m,
                           Lhs::g + Rhs::g,
                           Lhs::s + Rhs::s,
                           Lhs::A + Rhs::A,
                           Lhs::K + Rhs::K,
                           Lhs::mol + Rhs::mol,
                           Lhs::cd + Rhs::cd>;

template <typename Lhs, typename Rhs>
using base_divide = base<Lhs::m - Rhs::m,
                         Lhs::g - Rhs::g,
                         Lhs::s - Rhs::s,
                         Lhs::A - Rhs::A,
                         Lhs::K - Rhs::K,
                         Lhs::mol - Rhs::mol,
                         Lhs::cd - Rhs::cd>;

template <typename Lhs>
using base_inverse = base<-Lhs::m,
                          -Lhs::g,
                          -Lhs::s,
                          -Lhs::A,
                          -Lhs::K,
                          -Lhs::mol,
                          -Lhs::cd>;

// why is this not in the stdandard?
template <typename T, typename U>
struct implication
    : std::disjunction<std::negation<T>, U>
{};

template <typename T, typename U>
inline constexpr bool implication_v = implication<T, U>::value;
} // namespace detail

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

    template<typename _Rep2, typename _Ratio2, typename _Base2,
             class = std::enable_if_t<detail::implication_v<std::is_floating_point<_Rep2>,
                                                            std::is_floating_point<rep>>>>
    constexpr unit(const unit<_Rep2, _Ratio2, _Base2> &other)
        : _count(unit_cast<unit>(other).count()) { }

    constexpr rep count() const { return _count; }

private:
    rep _count = 0;
};

// partial specialisation for "time"
// this allows us to add conversion operators to std::chrono
template<typename _Rep, typename _Ratio>
struct unit<_Rep, _Ratio, detail::_s<1>>
{
    using rep   = _Rep;
    using ratio = _Ratio;
    using base  = detail::_s<1>;

    constexpr unit() = default;
    explicit constexpr unit(rep count)
        : _count(count) { }

    template<typename _Rep2, typename _Ratio2, typename _Base2>
    constexpr unit(const unit<_Rep2, _Ratio2, _Base2> &other)
        : _count(unit_cast<unit>(other).count()) { }

    constexpr rep count() const { return _count; }

    template<typename _Rep2, typename _Ratio2>
    operator std::chrono::duration<_Rep2, _Ratio2>() {
        using common_rep   = std::common_type_t<rep, _Rep2>;
        using common_ratio = std::common_type_t<ratio, _Ratio2>;
        using common_unit = unit<common_rep, common_ratio, base>;
        return std::chrono::duration<common_rep, common_ratio>{ common_unit(*this).count() };
    }

private:
    rep _count = 0;
};

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator==(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using u1 = unit<_Rep1, _Ratio1, _Base1>;
    using u2 = unit<_Rep2, _Ratio2, _Base2>;
    using common_unit = typename std::common_type_t<u1, u2>;

    return common_unit{lhs}.count() == common_unit{rhs}.count();
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator<(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using u1 = unit<_Rep1, _Ratio1, _Base1>;
    using u2 = unit<_Rep2, _Ratio2, _Base2>;
    using common_unit = typename std::common_type_t<u1, u2>;

    return common_unit{lhs}.count() < common_unit{rhs}.count();
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator!=(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    return !(lhs == rhs);
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator<=(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    return !(rhs < lhs);
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator>(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    return rhs < lhs;
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator>=(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                          const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    return !(lhs < rhs);
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator+(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using u1 = unit<_Rep1, _Ratio1, _Base1>;
    using u2 = unit<_Rep2, _Ratio2, _Base2>;
    using common_unit = typename std::common_type_t<u1, u2>;

    return common_unit{common_unit{lhs}.count() + common_unit{rhs}.count()};
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2,
          class = std::enable_if_t<std::is_same<_Base1, _Base2>::value>>
constexpr auto operator-(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using u1 = unit<_Rep1, _Ratio1, _Base1>;
    using u2 = unit<_Rep2, _Ratio2, _Base2>;
    using common_unit = typename std::common_type_t<u1, u2>;

    return common_unit{common_unit{lhs}.count() - common_unit{rhs}.count()};
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2>
constexpr auto operator*(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using common_base = detail::base_multiply<_Base1, _Base2>;
    using common_ratio = typename std::common_type_t<_Ratio1, _Ratio2>;
    using common_rep = typename std::common_type_t<_Rep1, _Rep2>;

    return unit<common_rep, common_ratio, common_base>{lhs.count() * rhs.count()};
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2>
constexpr auto operator*(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const _Rep2 &rhs)
{
    using common_unit = unit<std::common_type_t<_Rep1, _Rep2>, _Ratio1, _Base1>;
    return common_unit{common_unit{lhs}.count() * rhs};
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2>
constexpr auto operator*(const _Rep2 &lhs,
                         const unit<_Rep1, _Ratio1, _Base1> &rhs)
{
    return rhs * lhs;
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2, typename _Ratio2, typename _Base2>
constexpr auto operator/(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const unit<_Rep2, _Ratio2, _Base2> &rhs)
{
    using common_base = detail::base_divide<_Base1, _Base2>;
    using common_ratio = typename std::common_type_t<_Ratio1, _Ratio2>;
    using common_rep = typename std::common_type_t<_Rep1, _Rep2>;

    return unit<common_rep, common_ratio, common_base>{lhs.count() / rhs.count()};
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2>
constexpr auto operator/(const unit<_Rep1, _Ratio1, _Base1> &lhs,
                         const _Rep2 &rhs)
{
    using common_rep = std::common_type_t<_Rep1, _Rep2>;
    using common_unit = unit<common_rep, _Ratio1, _Base1>;
    return common_unit{common_unit{lhs}.count() / rhs};
}

template <typename _Rep1, typename _Ratio1, typename _Base1,
          typename _Rep2>
constexpr auto operator/(const _Rep2 &lhs,
                         const unit<_Rep1, _Ratio1, _Base1> &rhs)
{
    using common_rep = std::common_type_t<_Rep1, _Rep2>;
    using common_unit = unit<common_rep, _Ratio1, detail::base_inverse<_Base1>>;
    return common_unit{lhs / rhs.count()};
}

#define UNIT_TEMPLATE template<typename _Rep, typename _Ratio = std::ratio<1>>
// base units (base unit for mass is kg, not gram)
template<typename _Rep, typename _Ratio = std::ratio<1000, 1>>
using mass = unit<_Rep, _Ratio, detail::_g<1>>;
UNIT_TEMPLATE using length             = unit<_Rep, _Ratio, detail::_m<1>>;
UNIT_TEMPLATE using time               = unit<_Rep, _Ratio, detail::_s<1>>;
UNIT_TEMPLATE using current            = unit<_Rep, _Ratio, detail::_A<1>>;
UNIT_TEMPLATE using temperature        = unit<_Rep, _Ratio, detail::_K<1>>;
UNIT_TEMPLATE using amount             = unit<_Rep, _Ratio, detail::_mol<1>>;
UNIT_TEMPLATE using luminous_intensity = unit<_Rep, _Ratio, detail::_cd<1>>;
// derived units
UNIT_TEMPLATE using area                   = unit<_Rep, _Ratio, detail::_m<2>>;
UNIT_TEMPLATE using volume                 = unit<_Rep, _Ratio, detail::_m<3>>;
UNIT_TEMPLATE using velocity               = decltype(length<_Rep, _Ratio>{} / time<_Rep, _Ratio>{});
UNIT_TEMPLATE using acceleration           = decltype(velocity<_Rep, _Ratio>{} / time<_Rep, _Ratio>{});
UNIT_TEMPLATE using angle                  = unit<_Rep, _Ratio, detail::base<>>;
UNIT_TEMPLATE using solid_angle            = unit<_Rep, _Ratio, detail::base<>>;
UNIT_TEMPLATE using frequency              = decltype(1 / time<_Rep, _Ratio>{});
UNIT_TEMPLATE using force                  = unit<_Rep, _Ratio, detail::base<1, 1, -2>>;
UNIT_TEMPLATE using pressure               = decltype(force<_Rep, _Ratio>{} / area<_Rep, _Ratio>{});
UNIT_TEMPLATE using energy                 = decltype(force<_Rep, _Ratio>{} * length<_Rep, _Ratio>{});
UNIT_TEMPLATE using power                  = decltype(energy<_Rep, _Ratio>{} / time<_Rep, _Ratio>{});
UNIT_TEMPLATE using electric_charge        = decltype(time<_Rep, _Ratio>{} * current<_Rep, _Ratio>{});
UNIT_TEMPLATE using voltage                = decltype(power<_Rep, _Ratio>{} / current<_Rep, _Ratio>{});
UNIT_TEMPLATE using capacitance            = decltype(electric_charge<_Rep, _Ratio>{} / voltage<_Rep, _Ratio>{});
UNIT_TEMPLATE using electric_resistance    = decltype(voltage<_Rep, _Ratio>{} / current<_Rep, _Ratio>{});
UNIT_TEMPLATE using electrical_conductance = decltype(current<_Rep, _Ratio>{} / voltage<_Rep, _Ratio>{});
UNIT_TEMPLATE using magnetic_flux          = decltype(voltage<_Rep, _Ratio>{} * time<_Rep, _Ratio>{});
UNIT_TEMPLATE using magnetic_flux_density  = decltype(magnetic_flux<_Rep, _Ratio>{} / area<_Rep, _Ratio>{});
UNIT_TEMPLATE using inductance             = decltype(magnetic_flux<_Rep, _Ratio>{} / current<_Rep, _Ratio>{});
UNIT_TEMPLATE using luminous_flux          = decltype(luminous_intensity<_Rep, _Ratio>{} * solid_angle<_Rep, _Ratio>{});
UNIT_TEMPLATE using illuminance            = decltype(luminous_flux<_Rep, _Ratio>{} / area<_Rep, _Ratio>{});
UNIT_TEMPLATE using radioactivity          = unit<_Rep, _Ratio, detail::_s<-1>>;
UNIT_TEMPLATE using absorbed_dose          = decltype(energy<_Rep, _Ratio>{} / mass<_Rep, _Ratio>{});
UNIT_TEMPLATE using equivalent_dose        = decltype(energy<_Rep, _Ratio>{} / mass<_Rep, _Ratio>{});
UNIT_TEMPLATE using catalytic_activity     = decltype(amount<_Rep, _Ratio>{} / time<_Rep, _Ratio>{});
#undef UNIT_TEMPLATE

#define PREFIXES(unit, dim)         \
    PREFIXED_UNIT(atto,  unit, dim) \
    PREFIXED_UNIT(atto,  unit, dim) \
    PREFIXED_UNIT(femto, unit, dim) \
    PREFIXED_UNIT(pico,  unit, dim) \
    PREFIXED_UNIT(nano,  unit, dim) \
    PREFIXED_UNIT(micro, unit, dim) \
    PREFIXED_UNIT(milli, unit, dim) \
    PREFIXED_UNIT(centi, unit, dim) \
    PREFIXED_UNIT(deci,  unit, dim) \
    using unit = dim<int, std::ratio<1>>; \
    PREFIXED_UNIT(deca,  unit, dim) \
    PREFIXED_UNIT(hecto, unit, dim) \
    PREFIXED_UNIT(kilo,  unit, dim) \
    PREFIXED_UNIT(mega,  unit, dim) \
    PREFIXED_UNIT(giga,  unit, dim) \
    PREFIXED_UNIT(tera,  unit, dim) \
    PREFIXED_UNIT(peta,  unit, dim) \
    PREFIXED_UNIT(exa,   unit, dim)

#define PREFIXED_UNIT(prefix, unit, dim) using prefix ## unit = dim<int, std::prefix>;

PREFIXES(meter, length)
PREFIXES(gram, mass)
PREFIXES(second, time)
PREFIXES(ampere, current)
PREFIXES(kelvin, temperature)
PREFIXES(mole, amount)
PREFIXES(candela, luminous_intensity)
#undef PREFIXES
#undef PREFIXED_UNIT
} // namespace si
