#pragma once

#include "tokens.h"

#include <cstddef>
#include <random>
#include <span>
#include <sstream>

[[nodiscard]] inline auto
randEngine() -> std::mt19937&
{
    static std::mt19937 r = [] {
        std::random_device rd;
        return std::mt19937{rd()};
    }();
    return r;
}

[[nodiscard]] inline auto
random_token_type_and_size() -> std::tuple<ripple::TokenType, std::size_t>
{
    using namespace ripple;
    auto& rng = randEngine();
    std::uniform_int_distribution<> d(0, 8);
    switch (d(rng))
    {
        using enum ripple::TokenType;
        case 0:
            return {None, 20};
        case 1:
            return {NodePublic, 32};
        case 2:
            return {NodePublic, 33};
        case 3:
            return {NodePrivate, 32};
        case 4:
            return {AccountID, 20};
        case 5:
            return {AccountPublic, 32};
        case 6:
            return {AccountPublic, 33};
        case 7:
            return {AccountSecret, 32};
        case 8:
            return {FamilySeed, 16};
        default:
            assert(0);
            return {AccountPublic, 33};
    }
}

// Return the token type and subspan of `d` to use as test data.
[[nodiscard]] inline auto
random_b256_test_data(std::span<std::uint8_t> d)
    -> std::tuple<ripple::TokenType, std::span<std::uint8_t>>
{
    auto& rng = randEngine();
    std::uniform_int_distribution<std::uint8_t> dist(0, 255);
    auto [tok_type, tok_size] = random_token_type_and_size();
    std::generate(d.begin(), d.begin() + tok_size, [&] { return dist(rng); });
    return {tok_type, d.subspan(0, tok_size)};
}

inline auto
print_as_char(std::span<std::uint8_t> a, std::span<std::uint8_t> b)
{
    auto as_string = [](std::span<std::uint8_t> s) {
        std::string r;
        r.resize(s.size());
        std::copy(s.begin(), s.end(), r.begin());
        return r;
    };
    auto sa = as_string(a);
    auto sb = as_string(b);
    std::cerr << "\n\n" << sa << "\n" << sb << "\n";
}

inline auto
print_as_int(std::span<std::uint8_t> a, std::span<std::uint8_t> b)
{
    auto as_string = [](std::span<std::uint8_t> s) -> std::string {
        std::stringstream sstr;
        for (auto i : s)
        {
            sstr << std::setw(3) << int(i) << ',';
        }
        return sstr.str();
    };
    auto sa = as_string(a);
    auto sb = as_string(b);
    std::cerr << "\n\n" << sa << "\n" << sb << "\n";
}
