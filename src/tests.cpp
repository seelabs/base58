#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "b58_utils.h"
#include "test_utils.h"
#include "tokens.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>

#include <array>
#include <random>
#include <span>

#ifndef _MSC_VER

namespace multiprecision_utils {
// return a random bigint of the same number in two forms. The first is smallest
// coeff first u64. The second is boost multiprecision representation.

inline auto
randEngine() -> std::mt19937&
{
    static std::mt19937 r = [] {
        std::random_device rd;
        return std::mt19937{rd()};
    }();
    return r;
}

boost::multiprecision::checked_uint512_t
to_boost_mp(std::span<std::uint64_t> in)
{
    boost::multiprecision::checked_uint512_t mbp = 0;
    for (auto i = in.rbegin(); i != in.rend(); ++i)
    {
        mbp <<= 64;
        mbp += *i;
    }
    return mbp;
}

std::vector<std::uint64_t>
random_bigint(std::uint8_t minSize = 1, std::uint8_t maxSize = 5)
{
    auto eng = randEngine();
    std::uniform_int_distribution<std::uint8_t> numCoeffDist(minSize, maxSize);
    std::uniform_int_distribution<std::uint64_t> dist;
    auto const numCoeff = numCoeffDist(eng);
    std::vector<std::uint64_t> coeffs;
    coeffs.reserve(numCoeff);
    for (int i = 0; i < numCoeff; ++i)
    {
        coeffs.push_back(dist(eng));
    }
    return coeffs;
}
}  // namespace multiprecision_utils

TEST_CASE("Multiprecision math matches boost", "[multiprecision]")
{
    // Generate a random boost multiprecision
    // Import the bytes into
    using namespace boost::random;
    using namespace boost::multiprecision;

    constexpr std::size_t iters = 1000000;
    auto eng = randEngine();
    std::uniform_int_distribution<std::uint64_t> dist;
    for (int i = 0; i < iters; ++i)
    {
        std::uint64_t const d = dist(eng);
        if (!d)
            continue;
        auto big_int = multiprecision_utils::random_bigint();
        auto boost_big_int = multiprecision_utils::to_boost_mp(
            std::span<std::uint64_t>(big_int.data(), big_int.size()));

        auto ref_div = boost_big_int / d;
        auto ref_mod = boost_big_int % d;

        auto mod = b58_fast::detail::inplace_bigint_div_rem(
            std::span<uint64_t>(big_int.data(), big_int.size()), d);
        auto found_div = multiprecision_utils::to_boost_mp(big_int);
        REQUIRE(ref_mod.convert_to<std::uint64_t>() == mod);
        REQUIRE(found_div == ref_div);
    }
    for (int i = 0; i < iters; ++i)
    {
        std::uint64_t const d = dist(eng);
        auto big_int = multiprecision_utils::random_bigint(/*minSize*/ 2);
        if (big_int[big_int.size() - 1] ==
            std::numeric_limits<std::uint64_t>::max())
        {
            big_int[big_int.size() - 1] -= 1;  // Prevent overflow
        }
        auto boost_big_int = multiprecision_utils::to_boost_mp(
            std::span<std::uint64_t>(big_int.data(), big_int.size()));

        auto ref_add = boost_big_int + d;

        b58_fast::detail::inplace_bigint_add(
            std::span<uint64_t>(big_int.data(), big_int.size()), d);
        auto found_add = multiprecision_utils::to_boost_mp(big_int);
        REQUIRE(ref_add == found_add);
    }
    for (int i = 0; i < iters; ++i)
    {
        std::uint64_t const d = dist(eng);
        auto big_int = multiprecision_utils::random_bigint(/* minSize */ 2);
        // inplace mul requires the most significant coeff to be zero to hold
        // the result.
        big_int[big_int.size() - 1] = 0;
        auto boost_big_int = multiprecision_utils::to_boost_mp(
            std::span<std::uint64_t>(big_int.data(), big_int.size()));

        auto ref_mul = boost_big_int * d;

        b58_fast::detail::inplace_bigint_mul(
            std::span<uint64_t>(big_int.data(), big_int.size()), d);
        auto found_mul = multiprecision_utils::to_boost_mp(big_int);
        REQUIRE(ref_mul == found_mul);
    }
}

TEST_CASE("New encode implementation match reference", "[b58_fast]")
{
    std::array<std::uint8_t, 128> b256DataBuf;
    std::array<std::uint8_t, 64> b58ResultBuf[2];
    std::array<std::span<std::uint8_t>, 2> b58Result;

    std::array<std::uint8_t, 64> b256ResultBuf[2];
    std::array<std::span<std::uint8_t>, 2> b256Result;

    constexpr std::size_t iters = 1000000;
    for (int i = 0; i < iters; ++i)
    {
        auto [tokType, b256Data] = random_b256_test_data(
            std::span(b256DataBuf.data(), b256DataBuf.size()));
        for (int j = 0; j < 2; ++j)
        {
            auto outBuf =
                std::span(b58ResultBuf[j].data(), b58ResultBuf[j].size());
            if (j == 0)
            {
                auto r = ripple::b58_fast::encodeBase58Token(
                    tokType, b256Data, outBuf);
                REQUIRE(r);
                b58Result[j] = r.value();
            }
            else
            {
                auto s = ripple::b58_ref::encodeBase58Token(
                    tokType, b256Data.data(), b256Data.size());
                REQUIRE(s.size());
                b58Result[j] = outBuf.subspan(0, s.size());
                std::copy(s.begin(), s.end(), b58Result[j].begin());
            }
        }
        REQUIRE(b58Result[0].size() == b58Result[1].size());
        for (int b = 0, e = b58Result[0].size(); b != e; ++b)
        {
            if (b58Result[0][b] != b58Result[1][b])
            {
                print_as_char(b58Result[0], b58Result[1]);
            }

            REQUIRE(b58Result[0][b] == b58Result[1][b]);
        }

        for (int j = 0; j < 2; ++j)
        {
            auto outBuf =
                std::span(b256ResultBuf[j].data(), b256ResultBuf[j].size());
            if (j == 0)
            {
                std::string in(
                    b58Result[j].data(),
                    b58Result[j].data() + b58Result[j].size());
                auto r =
                    ripple::b58_fast::decodeBase58Token(tokType, in, outBuf);
                if (!r)
                {
                    std::cerr << r.error().message() << "\n";
                }
                REQUIRE(r);
                b256Result[j] = r.value();
            }
            else
            {
                std::string st(b58Result[j].begin(), b58Result[j].end());
                auto s = ripple::b58_ref::decodeBase58Token(st, tokType);
                REQUIRE(s.size());
                b256Result[j] = outBuf.subspan(0, s.size());
                std::copy(s.begin(), s.end(), b256Result[j].begin());
            }
        }

        REQUIRE(b256Result[0].size() == b256Result[1].size());
        for (int b = 0, e = b256Result[0].size(); b != e; ++b)
        {
            if (b256Result[0][b] != b256Result[1][b])
            {
                print_as_int(b256Result[0], b256Result[1]);
            }

            REQUIRE(b256Result[0][b] == b256Result[1][b]);
        }
    }
}

#endif
