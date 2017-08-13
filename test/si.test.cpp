#include <catch.hpp>

#include "si/si.hpp"

#include <type_traits>

// for testing purposes we declare a convenience type with some sane defaults
template<typename _Rep = int, typename _Ratio = std::ratio<1>, typename _Base = si::detail::base<>>
using test_unit = si::unit<_Rep, _Ratio, _Base>;

TEST_CASE("Base instantiations", "[detail]")
{
    using namespace si::detail;
    CHECK((std::is_same<base<1, 0, 0, 0, 0, 0, 0>, _m<>>::value));
    CHECK((std::is_same<base<0, 1, 0, 0, 0, 0, 0>, _g<>>::value));
    CHECK((std::is_same<base<0, 0, 1, 0, 0, 0, 0>, _s<>>::value));
    CHECK((std::is_same<base<0, 0, 0, 1, 0, 0, 0>, _A<>>::value));
    CHECK((std::is_same<base<0, 0, 0, 0, 1, 0, 0>, _K<>>::value));
    CHECK((std::is_same<base<0, 0, 0, 0, 0, 1, 0>, _mol<>>::value));
    CHECK((std::is_same<base<0, 0, 0, 0, 0, 0, 1>, _cd<>>::value));
}

TEST_CASE("Base operations", "[detail]")
{
    using namespace si::detail;
    SECTION("Multiplying") {
        CHECK((std::is_same<base_multiply<_m<1>,  _m<2>>, base<3>>::value));
        CHECK((std::is_same<base_multiply<_m<-3>, _m<2>>, base<-1>>::value));
        CHECK((std::is_same<base_multiply<_g<1>,  _m<2>>, base<2, 1>>::value));
        CHECK((std::is_same<base_multiply<_m<2>,  _g<1>>, base<2, 1>>::value));
    }
    SECTION("Divide") {
        CHECK((std::is_same<base_divide<_m<3>,  _m<2>>, base<1>>::value));
        CHECK((std::is_same<base_divide<_m<-3>, _m<2>>, base<-5>>::value));
        CHECK((std::is_same<base_divide<_g<1>,  _m<2>>, base<-2, 1>>::value));
        CHECK((std::is_same<base_divide<_m<2>,  _g<1>>, base<2, -1>>::value));
    }
}

TEST_CASE("Instanciate derived units", "[detail]")
{
    using namespace si::detail;

}

TEST_CASE("radian and steradian are different types", "[!shouldfail][!hide][detail]")
{
    using namespace si::detail;
    CHECK((!std::is_same<_rad<>, _sr<>>::value));
}

TEST_CASE("Unit Constructors", "[unit][constructors]")
{
    SECTION("Default constructors initialize to 0")
    {
        CHECK(test_unit<>{}.count() == 0);
    }
    SECTION("Value constructors set count correctly")
    {
        auto values = {
            std::numeric_limits<int>::min(),
            0,
            std::numeric_limits<int>::max()
        };

        for (auto v : values) {
            CHECK(test_unit<int>{v}.count() == v);
        }
    }
    SECTION("Value constructors work with floating types too")
    {
        auto values = {
            std::numeric_limits<float>::min(),
            0.0f,
            std::numeric_limits<float>::max()};

        for (auto v : values) {
            CHECK(test_unit<float>{v}.count() == v);
        }
    }
}

TEST_CASE("Unit Representation", "[unit][repr]")
{
    CHECK((std::is_same<decltype(test_unit<int8_t>{}.count()),   int8_t>::value));
    CHECK((std::is_same<decltype(test_unit<int16_t>{}.count()),  int16_t>::value));
    CHECK((std::is_same<decltype(test_unit<int32_t>{}.count()),  int32_t>::value));
    CHECK((std::is_same<decltype(test_unit<int64_t>{}.count()),  int64_t>::value));
    CHECK((std::is_same<decltype(test_unit<uint8_t>{}.count()),  uint8_t>::value));
    CHECK((std::is_same<decltype(test_unit<uint16_t>{}.count()), uint16_t>::value));
    CHECK((std::is_same<decltype(test_unit<uint32_t>{}.count()), uint32_t>::value));
    CHECK((std::is_same<decltype(test_unit<uint64_t>{}.count()), uint64_t>::value));
    CHECK((std::is_same<decltype(test_unit<float>{}.count()),    float>::value));
    CHECK((std::is_same<decltype(test_unit<double>{}.count()),   double>::value));
}

TEST_CASE("Unit comparison operators", "[unit][operators]")
{
    SECTION("==")
    {
        CHECK((test_unit<>{}) == (test_unit<>{}));
        CHECK((test_unit<int>{1}) == (test_unit<int>{1}));
        CHECK((test_unit<int, std::milli>{3}) == (test_unit<int, std::milli>{3}));
        CHECK((test_unit<int, std::deca>{1}) == (test_unit<int>{10}));
        CHECK((test_unit<unsigned>{}) == (test_unit<int>{}));
        CHECK((test_unit<double>{0.1}) == (test_unit<double>{0.1}));

        CHECK_FALSE((test_unit<>{}) == (test_unit<>{1}));
        CHECK_FALSE((test_unit<int>{1}) == (test_unit<int>{0}));
        CHECK_FALSE((test_unit<int, std::deci>{3}) == (test_unit<int, std::milli>{3}));
        CHECK_FALSE((test_unit<int, std::deca>{1}) == (test_unit<int>{1}));
        CHECK_FALSE((test_unit<double>{0.1}) == (test_unit<double>{0.2}));
    }
    SECTION("!=")
    {
        CHECK((test_unit<>{}) != (test_unit<>{1}));
        CHECK((test_unit<int>{1}) != (test_unit<int>{0}));
        CHECK((test_unit<int, std::deci>{3}) != (test_unit<int, std::milli>{3}));
        CHECK((test_unit<int, std::deca>{1}) != (test_unit<int>{1}));
        CHECK((test_unit<double>{0.1}) != (test_unit<double>{0.2}));

        CHECK_FALSE((test_unit<>{}) != (test_unit<>{}));
        CHECK_FALSE((test_unit<int>{1}) != (test_unit<int>{1}));
        CHECK_FALSE((test_unit<int, std::milli>{3}) != (test_unit<int, std::milli>{3}));
        CHECK_FALSE((test_unit<int, std::deca>{1}) != (test_unit<int>{10}));
        CHECK_FALSE((test_unit<unsigned>{}) != (test_unit<int>{}));
        CHECK_FALSE((test_unit<double>{0.1}) != (test_unit<double>{0.1}));
    }
    SECTION(">")
    {
        CHECK((test_unit<int>{1}) > (test_unit<int>{0}));
        CHECK((test_unit<int, std::deci>{3}) > (test_unit<int, std::milli>{3}));
        CHECK((test_unit<int, std::deca>{1}) > (test_unit<int>{1}));
        CHECK((test_unit<double>{0.3}) > (test_unit<double>{0.2}));

        CHECK_FALSE((test_unit<int>{0}) > (test_unit<int>{1}));
        CHECK_FALSE((test_unit<int, std::milli>{3}) > (test_unit<int, std::deci>{3}));
        CHECK_FALSE((test_unit<int>{1}) > (test_unit<int, std::deca>{1}));
        CHECK_FALSE((test_unit<double>{0.2}) > (test_unit<double>{0.3}));
    }
    SECTION(">=")
    {
        CHECK((test_unit<>{}) >= (test_unit<>{}));
        CHECK((test_unit<int>{1}) >= (test_unit<int>{1}));
        CHECK((test_unit<int>{1}) >= (test_unit<int>{0}));
        CHECK((test_unit<int, std::deci>{3}) >= (test_unit<int, std::milli>{3}));
        CHECK((test_unit<int, std::deca>{1}) >= (test_unit<int>{1}));
        CHECK((test_unit<double>{0.3}) >= (test_unit<double>{0.2}));

        CHECK_FALSE((test_unit<int>{0}) >= (test_unit<int>{1}));
        CHECK_FALSE((test_unit<int, std::milli>{3}) >= (test_unit<int, std::deci>{3}));
        CHECK_FALSE((test_unit<int>{1}) >= (test_unit<int, std::deca>{1}));
        CHECK_FALSE((test_unit<double>{0.2}) >= (test_unit<double>{0.3}));
    }
    SECTION("<")
    {
        CHECK((test_unit<int>{0}) < (test_unit<int>{1}));
        CHECK((test_unit<int, std::milli>{3}) < (test_unit<int, std::deci>{3}));
        CHECK((test_unit<int>{1}) < (test_unit<int, std::deca>{1}));
        CHECK((test_unit<double>{0.2}) < (test_unit<double>{0.3}));

        CHECK_FALSE((test_unit<int>{1}) < (test_unit<int>{0}));
        CHECK_FALSE((test_unit<int, std::deci>{3}) < (test_unit<int, std::milli>{3}));
        CHECK_FALSE((test_unit<int, std::deca>{1}) < (test_unit<int>{1}));
        CHECK_FALSE((test_unit<double>{0.3}) < (test_unit<double>{0.2}));
    }
    SECTION("<=")
    {
        CHECK((test_unit<>{}) <= (test_unit<>{}));
        CHECK((test_unit<int>{0}) <= (test_unit<int>{1}));
        CHECK((test_unit<int>{1}) <= (test_unit<int>{1}));
        CHECK((test_unit<int, std::milli>{3}) <= (test_unit<int, std::deci>{3}));
        CHECK((test_unit<int>{1}) <= (test_unit<int, std::deca>{1}));
        CHECK((test_unit<double>{0.2}) <= (test_unit<double>{0.3}));

        CHECK_FALSE((test_unit<int>{1}) <= (test_unit<int>{0}));
        CHECK_FALSE((test_unit<int, std::deci>{3}) <= (test_unit<int, std::milli>{3}));
        CHECK_FALSE((test_unit<int, std::deca>{1}) <= (test_unit<int>{1}));
        CHECK_FALSE((test_unit<double>{0.3}) <= (test_unit<double>{0.2}));
    }
}

TEST_CASE("Unit operations", "[unit][operators]")
{
    SECTION("Operations on the same repr and ratio")
    {
        SECTION("+")
        {
            CHECK((test_unit<>{1} + test_unit<>{1}) == (test_unit<>{2}));
            CHECK((test_unit<unsigned>{1} + test_unit<unsigned>{1}) == (test_unit<unsigned>{2}));
        }
        SECTION("-")
        {
            CHECK((test_unit<>{1} - test_unit<>{1}) == (test_unit<>{0}));
            CHECK((test_unit<unsigned>{1} - test_unit<unsigned>{1}) == (test_unit<unsigned>{0}));
        }
    }
    SECTION("Operations on different repr but same ratio")
    {
        SECTION("+")
        {
            CHECK((test_unit<>{1} + test_unit<unsigned>{1}) == (test_unit<unsigned>{2}));
            CHECK((test_unit<unsigned>{1} + test_unit<double>{1}) == (test_unit<double>{2}));
            CHECK((test_unit<float>{1} + test_unit<double>{1}) == (test_unit<double>{2}));
        }
        SECTION("-")
        {
            CHECK((test_unit<>{1} - test_unit<unsigned>{1}) == (test_unit<unsigned>{0}));
            CHECK((test_unit<unsigned>{1} - test_unit<double>{1}) == (test_unit<double>{0}));
            CHECK((test_unit<float>{1} - test_unit<double>{1}) == (test_unit<double>{0}));
        }
    }
    SECTION("Operations on different repr and ratio")
    {
        SECTION("+")
        {
            CHECK((test_unit<int, std::milli>{1} + test_unit<unsigned>{1}) == (test_unit<unsigned, std::milli>{1001}));
            CHECK((test_unit<unsigned, std::deca>{1} + test_unit<double, std::deci>{1}) == (test_unit<double>{10.1}));
            CHECK((test_unit<float>{1} + test_unit<double>{1}) == (test_unit<double>{2}));
        }
        SECTION("-")
        {
            CHECK((test_unit<int, std::milli>{1} - test_unit<unsigned>{1}) == (test_unit<int, std::milli>{-999}));
            CHECK((test_unit<unsigned, std::deca>{1} - test_unit<double, std::deci>{1}) == (test_unit<double>{9.9}));
            CHECK((test_unit<float>{1} - test_unit<double>{1}) == (test_unit<double>{0}));
        }
    }
}

TEST_CASE("Unit multiplication", "[unit][operators]")
{
    using test_unit1 = si::unit<int, std::ratio<1>, si::detail::base<1>>;
    using test_unit2 = si::unit<int, std::ratio<1>, si::detail::base<2>>;

    auto result1 = test_unit1{2} * test_unit1{4};
    CHECK(result1.count() == 8);
    CHECK((std::is_same<decltype(result1), test_unit2>::value));

    using test_unit01 = si::unit<int, std::ratio<1>, si::detail::base<0, 1>>;
    using test_unit11 = si::unit<int, std::ratio<1>, si::detail::base<1, 1>>;

    auto result2 = test_unit1{2} * test_unit01{-4};
    CHECK(result2.count() == -8);
    CHECK((std::is_same<decltype(result2), test_unit11>::value));
}

TEST_CASE("Unit division", "[unit][operators]")
{
    using test_unit1 = si::unit<double, std::ratio<1>, si::detail::base<1>>;
    using test_unit0 = si::unit<double, std::ratio<1>, si::detail::base<0>>;

    auto result1 = test_unit1{2} / test_unit1{4};
    CHECK(result1.count() == Approx(0.5));
    CHECK((std::is_same<decltype(result1), test_unit0>::value));

    using test_unit01 = si::unit<double, std::ratio<1>, si::detail::base<0, 1>>;
    using test_unit11 = si::unit<double, std::ratio<1>, si::detail::base<1, -1>>;

    auto result2 = test_unit1{2} / test_unit01{-4};
    CHECK(result2.count() == Approx(-0.5));
    CHECK((std::is_same<decltype(result2), test_unit11>::value));
}

TEST_CASE("Unit conversions", "[unit][unit_cast]")
{
    SECTION("Convert between different ratios")
    {
        test_unit<int> m{1};
        test_unit<int, std::milli> mm{m};
        REQUIRE(mm.count() == 1000);
    }

    SECTION("Convert down between different ratios")
    {
        test_unit<int, std::milli> mm{100};
        test_unit<int> m{mm};
        REQUIRE(m.count() == 0);
    }
}

TEST_CASE("Convenience types for base SI units", "[unit][detail][constructors]")
{
    SECTION("Default ratios")
    {
        CHECK((std::is_same<si::length<int>,             si::unit<int, std::ratio<1>, si::detail::_m<>>>::value));
        CHECK((std::is_same<si::mass<int>,               si::unit<int, std::ratio<1>, si::detail::_g<>>>::value));
        CHECK((std::is_same<si::time<int>,               si::unit<int, std::ratio<1>, si::detail::_s<>>>::value));
        CHECK((std::is_same<si::current<int>,            si::unit<int, std::ratio<1>, si::detail::_A<>>>::value));
        CHECK((std::is_same<si::temperature<int>,        si::unit<int, std::ratio<1>, si::detail::_K<>>>::value));
        CHECK((std::is_same<si::amount<int>,             si::unit<int, std::ratio<1>, si::detail::_mol<>>>::value));
        CHECK((std::is_same<si::luminous_intensity<int>, si::unit<int, std::ratio<1>, si::detail::_cd<>>>::value));
    }
    SECTION("Other ratios")
    {
        CHECK((std::is_same<si::length<int,             std::milli>, si::unit<int, std::milli, si::detail::_m<>>>::value));
        CHECK((std::is_same<si::mass<int,               std::deca>,  si::unit<int, std::deca,  si::detail::_g<>>>::value));
        CHECK((std::is_same<si::time<int,               std::exa>,   si::unit<int, std::exa,   si::detail::_s<>>>::value));
        CHECK((std::is_same<si::current<int,            std::atto>,  si::unit<int, std::atto,  si::detail::_A<>>>::value));
        CHECK((std::is_same<si::temperature<int,        std::hecto>, si::unit<int, std::hecto, si::detail::_K<>>>::value));
        CHECK((std::is_same<si::amount<int,             std::tera>,  si::unit<int, std::tera,  si::detail::_mol<>>>::value));
        CHECK((std::is_same<si::luminous_intensity<int, std::nano>,  si::unit<int, std::nano,  si::detail::_cd<>>>::value));
    }
}

TEST_CASE("Convenience types for derived SI units", "[unit][detail][constructors]")
{
    using namespace si::detail;
#define X(m, g, s, A, K, mol, cd) si::unit<int, std::ratio<1>, base< m, g, s, A, K, mol, cd>>
    CHECK((std::is_same<si::length<int>, X(1, 0, 0, 0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::area<int>,   X(2, 0, 0, 0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::volume<int>, X(3, 0, 0, 0, 0, 0, 0)>::value));

    CHECK((std::is_same<si::velocity<int>,     X(1, 0, -1, 0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::acceleration<int>, X(1, 0, -2, 0, 0, 0, 0)>::value));

    CHECK((std::is_same<si::angle<int>,                  X( 0,  0,  0,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::solid_angle<int>,            X( 0,  0,  0,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::frequency<int>,              X( 0,  0, -1,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::force<int>,                  X( 1,  1, -2,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::pressure<int>,               X(-1,  1, -2,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::energy<int>,                 X( 2,  1, -2,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::power<int>,                  X( 2,  1, -3,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::electric_charge<int>,        X( 0,  0,  1,  1, 0, 0, 0)>::value));
    CHECK((std::is_same<si::voltage<int>,                X( 2,  1, -3, -1, 0, 0, 0)>::value));
    CHECK((std::is_same<si::capacitance<int>,            X(-2, -1,  4,  2, 0, 0, 0)>::value));
    CHECK((std::is_same<si::electric_resistance<int>,    X( 2,  1, -3, -2, 0, 0, 0)>::value));
    CHECK((std::is_same<si::electrical_conductance<int>, X(-2, -1,  3,  2, 0, 0, 0)>::value));
    CHECK((std::is_same<si::magnetic_flux<int>,          X( 2,  1, -2, -1, 0, 0, 0)>::value));
    CHECK((std::is_same<si::magnetic_flux_density<int>,  X( 0,  1, -2, -1, 0, 0, 0)>::value));
    CHECK((std::is_same<si::inductance<int>,             X( 2,  1, -2, -2, 0, 0, 0)>::value));
    CHECK((std::is_same<si::luminous_flux<int>,          X( 0,  0,  0,  0, 0, 0, 1)>::value));
    CHECK((std::is_same<si::illuminance<int>,            X(-2,  0,  0,  0, 0, 0, 1)>::value));
    CHECK((std::is_same<si::radioactivity<int>,          X( 0,  0, -1,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::absorbed_dose<int>,          X( 2,  0, -2,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::equivalent_dose<int>,        X( 2,  0, -2,  0, 0, 0, 0)>::value));
    CHECK((std::is_same<si::catalytic_activity<int>,     X( 0,  0, -1,  0, 0, 1, 0)>::value));
#undef X
}
