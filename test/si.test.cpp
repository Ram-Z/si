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

TEST_CASE("Unit multiplication with different ratios", "[unit][operators]")
{
    using test_unit1 = si::unit<int, std::ratio<1>, si::detail::base<1>>;
    using test_unit2 = si::unit<int, std::ratio<1, 1000>, si::detail::base<2>>;
    using test_unit3 = si::unit<int, std::ratio<1, 1000>, si::detail::base<3>>;

    auto result1 = test_unit1{2} * test_unit2{4};
    CHECK(result1 == test_unit3{8});
    CHECK((std::is_same<decltype(result1), test_unit3>::value));

    using test_unit01 = si::unit<int, std::ratio<1>, si::detail::base<0, 1>>;
    using test_unit11 = si::unit<int, std::ratio<1000>, si::detail::base<1, 1>>;
    using test_unit12 = si::unit<int, std::ratio<1>, si::detail::base<1, 2>>;

    auto result2 = test_unit11{2} * test_unit01{-4};
    CHECK(result2 == test_unit12{-8});
    CHECK((std::is_same<decltype(result2), test_unit12>::value));
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

TEST_CASE("Unit division with different ratios", "[unit][operators]")
{
    using test_unit1 = si::unit<int, std::ratio<1, 1000>, si::detail::base<1>>;
    using test_unit2 = si::unit<int, std::ratio<1>, si::detail::base<2>>;
    using test_unit3 = si::unit<int, std::ratio<1, 1000>, si::detail::base<3>>;

    auto result1 = test_unit3{4} / test_unit2{2};
    CHECK(result1 == test_unit1{2});
    CHECK((std::is_same<decltype(result1), test_unit1>::value));

    using test_unit01 = si::unit<int, std::ratio<1>, si::detail::base<0, 1>>;
    using test_unit11 = si::unit<int, std::ratio<1000>, si::detail::base<1, 1>>;
    using test_unit10 = si::unit<int, std::ratio<1>, si::detail::base<1, 0>>;

    auto result2 = test_unit11{4} / test_unit01{-2};
    CHECK(result2 == test_unit10{-2});
    CHECK((std::is_same<decltype(result2), test_unit10>::value));
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

TEST_CASE("Prefixed Base Types", "[unit][constructors][detail]" )
{
    SECTION("length")
    {
        SECTION("attometer is length of ratio 1/1000000000000000000")
        {
            REQUIRE((std::is_same<si::attometer, si::length<int, std::ratio<1, 1000000000000000000>>>::value));
        }
        SECTION("femtometer is length of ratio 1/1000000000000000")
        {
            REQUIRE((std::is_same<si::femtometer, si::length<int, std::ratio<1, 1000000000000000>>>::value));
        }
        SECTION("picometer is length of ratio 1/1000000000000")
        {
            REQUIRE((std::is_same<si::picometer, si::length<int, std::ratio<1, 1000000000000>>>::value));
        }
        SECTION("nanometer is length of ratio 1/1000000000")
        {
            REQUIRE((std::is_same<si::nanometer, si::length<int, std::ratio<1, 1000000000>>>::value));
        }
        SECTION("micrometer is length of ratio 1/1000000")
        {
            REQUIRE((std::is_same<si::micrometer, si::length<int, std::ratio<1, 1000000>>>::value));
        }
        SECTION("millimeter is length of ratio 1/1000")
        {
            REQUIRE((std::is_same<si::millimeter, si::length<int, std::ratio<1, 1000>>>::value));
        }
        SECTION("centimeter is length of ratio 1/100")
        {
            REQUIRE((std::is_same<si::centimeter, si::length<int, std::ratio<1, 100>>>::value));
        }
        SECTION("decimeter is length of ratio 1/10")
        {
            REQUIRE((std::is_same<si::decimeter, si::length<int, std::ratio<1, 10>>>::value));
        }
        SECTION("meter is length of ratio 1/1")
        {
            REQUIRE((std::is_same<si::meter, si::length<int, std::ratio<1, 1>>>::value));
        }
        SECTION("decameter is length of ratio 10/1")
        {
            REQUIRE((std::is_same<si::decameter, si::length<int, std::ratio<10, 1>>>::value));
        }
        SECTION("hectometer is length of ratio 100/1")
        {
            REQUIRE((std::is_same<si::hectometer, si::length<int, std::ratio<100, 1>>>::value));
        }
        SECTION("kilometer is length of ratio 1000/1")
        {
            REQUIRE((std::is_same<si::kilometer, si::length<int, std::ratio<1000, 1>>>::value));
        }
        SECTION("megameter is length of ratio 1000000/1")
        {
            REQUIRE((std::is_same<si::megameter, si::length<int, std::ratio<1000000, 1>>>::value));
        }
        SECTION("gigameter is length of ratio 1000000000/1")
        {
            REQUIRE((std::is_same<si::gigameter, si::length<int, std::ratio<1000000000, 1>>>::value));
        }
        SECTION("terameter is length of ratio 1000000000000/1")
        {
            REQUIRE((std::is_same<si::terameter, si::length<int, std::ratio<1000000000000, 1>>>::value));
        }
        SECTION("petameter is length of ratio 1000000000000000/1")
        {
            REQUIRE((std::is_same<si::petameter, si::length<int, std::ratio<1000000000000000, 1>>>::value));
        }
        SECTION("exameter is length of ratio 1000000000000000000/1")
        {
            REQUIRE((std::is_same<si::exameter, si::length<int, std::ratio<1000000000000000000, 1>>>::value));
        }
    }

    SECTION("mass")
    {
        SECTION("attogram is mass of ratio 1/1000000000000000000")
        {
            REQUIRE((std::is_same<si::attogram, si::mass<int, std::ratio<1, 1000000000000000000>>>::value));
        }
        SECTION("femtogram is mass of ratio 1/1000000000000000")
        {
            REQUIRE((std::is_same<si::femtogram, si::mass<int, std::ratio<1, 1000000000000000>>>::value));
        }
        SECTION("picogram is mass of ratio 1/1000000000000")
        {
            REQUIRE((std::is_same<si::picogram, si::mass<int, std::ratio<1, 1000000000000>>>::value));
        }
        SECTION("nanogram is mass of ratio 1/1000000000")
        {
            REQUIRE((std::is_same<si::nanogram, si::mass<int, std::ratio<1, 1000000000>>>::value));
        }
        SECTION("microgram is mass of ratio 1/1000000")
        {
            REQUIRE((std::is_same<si::microgram, si::mass<int, std::ratio<1, 1000000>>>::value));
        }
        SECTION("milligram is mass of ratio 1/1000")
        {
            REQUIRE((std::is_same<si::milligram, si::mass<int, std::ratio<1, 1000>>>::value));
        }
        SECTION("centigram is mass of ratio 1/100")
        {
            REQUIRE((std::is_same<si::centigram, si::mass<int, std::ratio<1, 100>>>::value));
        }
        SECTION("decigram is mass of ratio 1/10")
        {
            REQUIRE((std::is_same<si::decigram, si::mass<int, std::ratio<1, 10>>>::value));
        }
        SECTION("gram is mass of ratio 1/1")
        {
            REQUIRE((std::is_same<si::gram, si::mass<int, std::ratio<1, 1>>>::value));
        }
        SECTION("decagram is mass of ratio 10/1")
        {
            REQUIRE((std::is_same<si::decagram, si::mass<int, std::ratio<10, 1>>>::value));
        }
        SECTION("hectogram is mass of ratio 100/1")
        {
            REQUIRE((std::is_same<si::hectogram, si::mass<int, std::ratio<100, 1>>>::value));
        }
        SECTION("kilogram is mass of ratio 1000/1")
        {
            REQUIRE((std::is_same<si::kilogram, si::mass<int, std::ratio<1000, 1>>>::value));
        }
        SECTION("megagram is mass of ratio 1000000/1")
        {
            REQUIRE((std::is_same<si::megagram, si::mass<int, std::ratio<1000000, 1>>>::value));
        }
        SECTION("gigagram is mass of ratio 1000000000/1")
        {
            REQUIRE((std::is_same<si::gigagram, si::mass<int, std::ratio<1000000000, 1>>>::value));
        }
        SECTION("teragram is mass of ratio 1000000000000/1")
        {
            REQUIRE((std::is_same<si::teragram, si::mass<int, std::ratio<1000000000000, 1>>>::value));
        }
        SECTION("petagram is mass of ratio 1000000000000000/1")
        {
            REQUIRE((std::is_same<si::petagram, si::mass<int, std::ratio<1000000000000000, 1>>>::value));
        }
        SECTION("exagram is mass of ratio 1000000000000000000/1")
        {
            REQUIRE((std::is_same<si::exagram, si::mass<int, std::ratio<1000000000000000000, 1>>>::value));
        }
    }

    SECTION("time")
    {
        SECTION("attosecond is time of ratio 1/1000000000000000000")
        {
            REQUIRE((std::is_same<si::attosecond, si::time<int, std::ratio<1, 1000000000000000000>>>::value));
        }
        SECTION("femtosecond is time of ratio 1/1000000000000000")
        {
            REQUIRE((std::is_same<si::femtosecond, si::time<int, std::ratio<1, 1000000000000000>>>::value));
        }
        SECTION("picosecond is time of ratio 1/1000000000000")
        {
            REQUIRE((std::is_same<si::picosecond, si::time<int, std::ratio<1, 1000000000000>>>::value));
        }
        SECTION("nanosecond is time of ratio 1/1000000000")
        {
            REQUIRE((std::is_same<si::nanosecond, si::time<int, std::ratio<1, 1000000000>>>::value));
        }
        SECTION("microsecond is time of ratio 1/1000000")
        {
            REQUIRE((std::is_same<si::microsecond, si::time<int, std::ratio<1, 1000000>>>::value));
        }
        SECTION("millisecond is time of ratio 1/1000")
        {
            REQUIRE((std::is_same<si::millisecond, si::time<int, std::ratio<1, 1000>>>::value));
        }
        SECTION("centisecond is time of ratio 1/100")
        {
            REQUIRE((std::is_same<si::centisecond, si::time<int, std::ratio<1, 100>>>::value));
        }
        SECTION("decisecond is time of ratio 1/10")
        {
            REQUIRE((std::is_same<si::decisecond, si::time<int, std::ratio<1, 10>>>::value));
        }
        SECTION("second is time of ratio 1/1")
        {
            REQUIRE((std::is_same<si::second, si::time<int, std::ratio<1, 1>>>::value));
        }
        SECTION("decasecond is time of ratio 10/1")
        {
            REQUIRE((std::is_same<si::decasecond, si::time<int, std::ratio<10, 1>>>::value));
        }
        SECTION("hectosecond is time of ratio 100/1")
        {
            REQUIRE((std::is_same<si::hectosecond, si::time<int, std::ratio<100, 1>>>::value));
        }
        SECTION("kilosecond is time of ratio 1000/1")
        {
            REQUIRE((std::is_same<si::kilosecond, si::time<int, std::ratio<1000, 1>>>::value));
        }
        SECTION("megasecond is time of ratio 1000000/1")
        {
            REQUIRE((std::is_same<si::megasecond, si::time<int, std::ratio<1000000, 1>>>::value));
        }
        SECTION("gigasecond is time of ratio 1000000000/1")
        {
            REQUIRE((std::is_same<si::gigasecond, si::time<int, std::ratio<1000000000, 1>>>::value));
        }
        SECTION("terasecond is time of ratio 1000000000000/1")
        {
            REQUIRE((std::is_same<si::terasecond, si::time<int, std::ratio<1000000000000, 1>>>::value));
        }
        SECTION("petasecond is time of ratio 1000000000000000/1")
        {
            REQUIRE((std::is_same<si::petasecond, si::time<int, std::ratio<1000000000000000, 1>>>::value));
        }
        SECTION("exasecond is time of ratio 1000000000000000000/1")
        {
            REQUIRE((std::is_same<si::exasecond, si::time<int, std::ratio<1000000000000000000, 1>>>::value));
        }
    }

    SECTION("current")
    {
        SECTION("attoampere is current of ratio 1/1000000000000000000")
        {
            REQUIRE((std::is_same<si::attoampere, si::current<int, std::ratio<1, 1000000000000000000>>>::value));
        }
        SECTION("femtoampere is current of ratio 1/1000000000000000")
        {
            REQUIRE((std::is_same<si::femtoampere, si::current<int, std::ratio<1, 1000000000000000>>>::value));
        }
        SECTION("picoampere is current of ratio 1/1000000000000")
        {
            REQUIRE((std::is_same<si::picoampere, si::current<int, std::ratio<1, 1000000000000>>>::value));
        }
        SECTION("nanoampere is current of ratio 1/1000000000")
        {
            REQUIRE((std::is_same<si::nanoampere, si::current<int, std::ratio<1, 1000000000>>>::value));
        }
        SECTION("microampere is current of ratio 1/1000000")
        {
            REQUIRE((std::is_same<si::microampere, si::current<int, std::ratio<1, 1000000>>>::value));
        }
        SECTION("milliampere is current of ratio 1/1000")
        {
            REQUIRE((std::is_same<si::milliampere, si::current<int, std::ratio<1, 1000>>>::value));
        }
        SECTION("centiampere is current of ratio 1/100")
        {
            REQUIRE((std::is_same<si::centiampere, si::current<int, std::ratio<1, 100>>>::value));
        }
        SECTION("deciampere is current of ratio 1/10")
        {
            REQUIRE((std::is_same<si::deciampere, si::current<int, std::ratio<1, 10>>>::value));
        }
        SECTION("ampere is current of ratio 1/1")
        {
            REQUIRE((std::is_same<si::ampere, si::current<int, std::ratio<1, 1>>>::value));
        }
        SECTION("decaampere is current of ratio 10/1")
        {
            REQUIRE((std::is_same<si::decaampere, si::current<int, std::ratio<10, 1>>>::value));
        }
        SECTION("hectoampere is current of ratio 100/1")
        {
            REQUIRE((std::is_same<si::hectoampere, si::current<int, std::ratio<100, 1>>>::value));
        }
        SECTION("kiloampere is current of ratio 1000/1")
        {
            REQUIRE((std::is_same<si::kiloampere, si::current<int, std::ratio<1000, 1>>>::value));
        }
        SECTION("megaampere is current of ratio 1000000/1")
        {
            REQUIRE((std::is_same<si::megaampere, si::current<int, std::ratio<1000000, 1>>>::value));
        }
        SECTION("gigaampere is current of ratio 1000000000/1")
        {
            REQUIRE((std::is_same<si::gigaampere, si::current<int, std::ratio<1000000000, 1>>>::value));
        }
        SECTION("teraampere is current of ratio 1000000000000/1")
        {
            REQUIRE((std::is_same<si::teraampere, si::current<int, std::ratio<1000000000000, 1>>>::value));
        }
        SECTION("petaampere is current of ratio 1000000000000000/1")
        {
            REQUIRE((std::is_same<si::petaampere, si::current<int, std::ratio<1000000000000000, 1>>>::value));
        }
        SECTION("exaampere is current of ratio 1000000000000000000/1")
        {
            REQUIRE((std::is_same<si::exaampere, si::current<int, std::ratio<1000000000000000000, 1>>>::value));
        }
    }

    SECTION("temperature")
    {
        SECTION("attokelvin is temperature of ratio 1/1000000000000000000")
        {
            REQUIRE((std::is_same<si::attokelvin, si::temperature<int, std::ratio<1, 1000000000000000000>>>::value));
        }
        SECTION("femtokelvin is temperature of ratio 1/1000000000000000")
        {
            REQUIRE((std::is_same<si::femtokelvin, si::temperature<int, std::ratio<1, 1000000000000000>>>::value));
        }
        SECTION("picokelvin is temperature of ratio 1/1000000000000")
        {
            REQUIRE((std::is_same<si::picokelvin, si::temperature<int, std::ratio<1, 1000000000000>>>::value));
        }
        SECTION("nanokelvin is temperature of ratio 1/1000000000")
        {
            REQUIRE((std::is_same<si::nanokelvin, si::temperature<int, std::ratio<1, 1000000000>>>::value));
        }
        SECTION("microkelvin is temperature of ratio 1/1000000")
        {
            REQUIRE((std::is_same<si::microkelvin, si::temperature<int, std::ratio<1, 1000000>>>::value));
        }
        SECTION("millikelvin is temperature of ratio 1/1000")
        {
            REQUIRE((std::is_same<si::millikelvin, si::temperature<int, std::ratio<1, 1000>>>::value));
        }
        SECTION("centikelvin is temperature of ratio 1/100")
        {
            REQUIRE((std::is_same<si::centikelvin, si::temperature<int, std::ratio<1, 100>>>::value));
        }
        SECTION("decikelvin is temperature of ratio 1/10")
        {
            REQUIRE((std::is_same<si::decikelvin, si::temperature<int, std::ratio<1, 10>>>::value));
        }
        SECTION("kelvin is temperature of ratio 1/1")
        {
            REQUIRE((std::is_same<si::kelvin, si::temperature<int, std::ratio<1, 1>>>::value));
        }
        SECTION("decakelvin is temperature of ratio 10/1")
        {
            REQUIRE((std::is_same<si::decakelvin, si::temperature<int, std::ratio<10, 1>>>::value));
        }
        SECTION("hectokelvin is temperature of ratio 100/1")
        {
            REQUIRE((std::is_same<si::hectokelvin, si::temperature<int, std::ratio<100, 1>>>::value));
        }
        SECTION("kilokelvin is temperature of ratio 1000/1")
        {
            REQUIRE((std::is_same<si::kilokelvin, si::temperature<int, std::ratio<1000, 1>>>::value));
        }
        SECTION("megakelvin is temperature of ratio 1000000/1")
        {
            REQUIRE((std::is_same<si::megakelvin, si::temperature<int, std::ratio<1000000, 1>>>::value));
        }
        SECTION("gigakelvin is temperature of ratio 1000000000/1")
        {
            REQUIRE((std::is_same<si::gigakelvin, si::temperature<int, std::ratio<1000000000, 1>>>::value));
        }
        SECTION("terakelvin is temperature of ratio 1000000000000/1")
        {
            REQUIRE((std::is_same<si::terakelvin, si::temperature<int, std::ratio<1000000000000, 1>>>::value));
        }
        SECTION("petakelvin is temperature of ratio 1000000000000000/1")
        {
            REQUIRE((std::is_same<si::petakelvin, si::temperature<int, std::ratio<1000000000000000, 1>>>::value));
        }
        SECTION("exakelvin is temperature of ratio 1000000000000000000/1")
        {
            REQUIRE((std::is_same<si::exakelvin, si::temperature<int, std::ratio<1000000000000000000, 1>>>::value));
        }
    }

    SECTION("amount")
    {
        SECTION("attomole is amount of ratio 1/1000000000000000000")
        {
            REQUIRE((std::is_same<si::attomole, si::amount<int, std::ratio<1, 1000000000000000000>>>::value));
        }
        SECTION("femtomole is amount of ratio 1/1000000000000000")
        {
            REQUIRE((std::is_same<si::femtomole, si::amount<int, std::ratio<1, 1000000000000000>>>::value));
        }
        SECTION("picomole is amount of ratio 1/1000000000000")
        {
            REQUIRE((std::is_same<si::picomole, si::amount<int, std::ratio<1, 1000000000000>>>::value));
        }
        SECTION("nanomole is amount of ratio 1/1000000000")
        {
            REQUIRE((std::is_same<si::nanomole, si::amount<int, std::ratio<1, 1000000000>>>::value));
        }
        SECTION("micromole is amount of ratio 1/1000000")
        {
            REQUIRE((std::is_same<si::micromole, si::amount<int, std::ratio<1, 1000000>>>::value));
        }
        SECTION("millimole is amount of ratio 1/1000")
        {
            REQUIRE((std::is_same<si::millimole, si::amount<int, std::ratio<1, 1000>>>::value));
        }
        SECTION("centimole is amount of ratio 1/100")
        {
            REQUIRE((std::is_same<si::centimole, si::amount<int, std::ratio<1, 100>>>::value));
        }
        SECTION("decimole is amount of ratio 1/10")
        {
            REQUIRE((std::is_same<si::decimole, si::amount<int, std::ratio<1, 10>>>::value));
        }
        SECTION("mole is amount of ratio 1/1")
        {
            REQUIRE((std::is_same<si::mole, si::amount<int, std::ratio<1, 1>>>::value));
        }
        SECTION("decamole is amount of ratio 10/1")
        {
            REQUIRE((std::is_same<si::decamole, si::amount<int, std::ratio<10, 1>>>::value));
        }
        SECTION("hectomole is amount of ratio 100/1")
        {
            REQUIRE((std::is_same<si::hectomole, si::amount<int, std::ratio<100, 1>>>::value));
        }
        SECTION("kilomole is amount of ratio 1000/1")
        {
            REQUIRE((std::is_same<si::kilomole, si::amount<int, std::ratio<1000, 1>>>::value));
        }
        SECTION("megamole is amount of ratio 1000000/1")
        {
            REQUIRE((std::is_same<si::megamole, si::amount<int, std::ratio<1000000, 1>>>::value));
        }
        SECTION("gigamole is amount of ratio 1000000000/1")
        {
            REQUIRE((std::is_same<si::gigamole, si::amount<int, std::ratio<1000000000, 1>>>::value));
        }
        SECTION("teramole is amount of ratio 1000000000000/1")
        {
            REQUIRE((std::is_same<si::teramole, si::amount<int, std::ratio<1000000000000, 1>>>::value));
        }
        SECTION("petamole is amount of ratio 1000000000000000/1")
        {
            REQUIRE((std::is_same<si::petamole, si::amount<int, std::ratio<1000000000000000, 1>>>::value));
        }
        SECTION("examole is amount of ratio 1000000000000000000/1")
        {
            REQUIRE((std::is_same<si::examole, si::amount<int, std::ratio<1000000000000000000, 1>>>::value));
        }
    }

    SECTION("luminous_intensity")
    {
        SECTION("attocandela is luminous_intensity of ratio 1/1000000000000000000")
        {
            REQUIRE((std::is_same<si::attocandela, si::luminous_intensity<int, std::ratio<1, 1000000000000000000>>>::value));
        }
        SECTION("femtocandela is luminous_intensity of ratio 1/1000000000000000")
        {
            REQUIRE((std::is_same<si::femtocandela, si::luminous_intensity<int, std::ratio<1, 1000000000000000>>>::value));
        }
        SECTION("picocandela is luminous_intensity of ratio 1/1000000000000")
        {
            REQUIRE((std::is_same<si::picocandela, si::luminous_intensity<int, std::ratio<1, 1000000000000>>>::value));
        }
        SECTION("nanocandela is luminous_intensity of ratio 1/1000000000")
        {
            REQUIRE((std::is_same<si::nanocandela, si::luminous_intensity<int, std::ratio<1, 1000000000>>>::value));
        }
        SECTION("microcandela is luminous_intensity of ratio 1/1000000")
        {
            REQUIRE((std::is_same<si::microcandela, si::luminous_intensity<int, std::ratio<1, 1000000>>>::value));
        }
        SECTION("millicandela is luminous_intensity of ratio 1/1000")
        {
            REQUIRE((std::is_same<si::millicandela, si::luminous_intensity<int, std::ratio<1, 1000>>>::value));
        }
        SECTION("centicandela is luminous_intensity of ratio 1/100")
        {
            REQUIRE((std::is_same<si::centicandela, si::luminous_intensity<int, std::ratio<1, 100>>>::value));
        }
        SECTION("decicandela is luminous_intensity of ratio 1/10")
        {
            REQUIRE((std::is_same<si::decicandela, si::luminous_intensity<int, std::ratio<1, 10>>>::value));
        }
        SECTION("candela is luminous_intensity of ratio 1/1")
        {
            REQUIRE((std::is_same<si::candela, si::luminous_intensity<int, std::ratio<1, 1>>>::value));
        }
        SECTION("decacandela is luminous_intensity of ratio 10/1")
        {
            REQUIRE((std::is_same<si::decacandela, si::luminous_intensity<int, std::ratio<10, 1>>>::value));
        }
        SECTION("hectocandela is luminous_intensity of ratio 100/1")
        {
            REQUIRE((std::is_same<si::hectocandela, si::luminous_intensity<int, std::ratio<100, 1>>>::value));
        }
        SECTION("kilocandela is luminous_intensity of ratio 1000/1")
        {
            REQUIRE((std::is_same<si::kilocandela, si::luminous_intensity<int, std::ratio<1000, 1>>>::value));
        }
        SECTION("megacandela is luminous_intensity of ratio 1000000/1")
        {
            REQUIRE((std::is_same<si::megacandela, si::luminous_intensity<int, std::ratio<1000000, 1>>>::value));
        }
        SECTION("gigacandela is luminous_intensity of ratio 1000000000/1")
        {
            REQUIRE((std::is_same<si::gigacandela, si::luminous_intensity<int, std::ratio<1000000000, 1>>>::value));
        }
        SECTION("teracandela is luminous_intensity of ratio 1000000000000/1")
        {
            REQUIRE((std::is_same<si::teracandela, si::luminous_intensity<int, std::ratio<1000000000000, 1>>>::value));
        }
        SECTION("petacandela is luminous_intensity of ratio 1000000000000000/1")
        {
            REQUIRE((std::is_same<si::petacandela, si::luminous_intensity<int, std::ratio<1000000000000000, 1>>>::value));
        }
        SECTION("exacandela is luminous_intensity of ratio 1000000000000000000/1")
        {
            REQUIRE((std::is_same<si::exacandela, si::luminous_intensity<int, std::ratio<1000000000000000000, 1>>>::value));
        }
    }
}
