#include <boost/outcome.hpp>
#include <boost/outcome/result.hpp>

#include <cassert>
#include <cinttypes>
#include <span>
#include <string>  // for logic error
#include <tuple>

#ifndef _MSC_VER
namespace b58_fast {
namespace detail {
template <class T>
using Result = boost::outcome_v2::result<T>;

/** Called when faulty logic causes a broken invariant. */
[[noreturn]] inline void
LogicError(std::string const& how) noexcept
{
    // dereference memory location zero
    int volatile* j = 0;
    (void)*j;
    std::abort();
}

// This optimizes to what hand written asm would do (single divide)
[[nodiscard]] inline std::tuple<std::uint64_t, std::uint64_t>
div_rem(std::uint64_t a, std::uint64_t b)
{
    return {a / b, a % b};
}

// Microsoft has _mul128
// This optimizes to what hand written asm would do (single multiply)
[[nodiscard]] inline std::tuple<std::uint64_t, std::uint64_t>
carrying_mul(std::uint64_t a, std::uint64_t b, std::uint64_t carry)
{
    unsigned __int128 x = a;
    unsigned __int128 y = b;
    unsigned __int128 c = x * y + carry;
    return {c & 0xffffffffffffffff, c >> 64};
}

[[nodiscard]] inline std::tuple<std::uint64_t, std::uint64_t>
carrying_add(std::uint64_t a, std::uint64_t b)
{
    unsigned __int128 x = a;
    unsigned __int128 y = b;
    unsigned __int128 c = x + y;
    return {c & 0xffffffffffffffff, c >> 64};
}

// Add a u64 to a "big uint" value inplace.
// The bitint value is stored with the smallest coeffiecients first
// (i.e a[0] is the 2^0 coefficient, a[n] is the 2^n coefficient)
// panics if overflows (this is a specialized adder for b58 decoding.
// it should never overflow).
inline void
inplace_bigint_add(std::span<std::uint64_t> a, std::uint64_t b)
{
    if (a.size() <= 1)
    {
        LogicError("Input span too small for inplace_bigint_add");
    }

    std::uint64_t carry;
    std::tie(a[0], carry) = carrying_add(a[0], b);

    for (auto& v : a.subspan(1))
    {
        if (!carry)
        {
            return;
        }
        std::tie(v, carry) = carrying_add(v, 1);
    }
    if (carry)
    {
        LogicError("Overflow in inplace_bigint_add");
    }
}

inline void
inplace_bigint_mul(std::span<std::uint64_t> a, std::uint64_t b)
{
    if (a.empty())
    {
        LogicError("Empty span passed to inplace_bigint_mul");
    }

    auto const last_index = a.size() - 1;
    if (a[last_index] != 0)
    {
        LogicError("Non-zero element in inplace_bigint_mul last index");
    }

    std::uint64_t carry = 0;
    for (auto& coeff : a.subspan(0, last_index))
    {
        std::tie(coeff, carry) = carrying_mul(coeff, b, carry);
    }
    a[last_index] = carry;
}
// divide a "big uint" value inplace and return the mod
// numerator is stored so smallest coefficients come first
[[nodiscard]] inline std::uint64_t
inplace_bigint_div_rem(std::span<uint64_t> numerator, std::uint64_t divisor)
{
    auto to_u128 = [](std::uint64_t high,
                      std::uint64_t low) -> unsigned __int128 {
        unsigned __int128 result = high;
        unsigned __int128 low128 = low;
        return ((result << 64) | low128);
    };
    auto div_rem_64 =
        [](unsigned __int128 num,
           std::uint64_t denom) -> std::tuple<std::uint64_t, std::uint64_t> {
        unsigned __int128 denom128 = denom;
        unsigned __int128 d = num / denom128;
        unsigned __int128 r = num % denom128;
        return {static_cast<std::uint64_t>(d), static_cast<std::uint64_t>(r)};
    };

    std::uint64_t prev_rem;
    std::size_t const last_index = numerator.size() - 1;
    std::tie(numerator[last_index], prev_rem) =
        div_rem(numerator[last_index], divisor);
    for (int i = last_index - 1; i >= 0; --i)
    {
        unsigned __int128 cur_num = to_u128(prev_rem, numerator[i]);
        std::tie(numerator[i], prev_rem) = div_rem_64(cur_num, divisor);
    }
    return prev_rem;
}

// convert from base 58^10 to base 58
// put largest coeffs first
[[nodiscard]] inline std::array<std::uint8_t, 10>
b58_10_to_b58_be(std::uint64_t input)
{
    constexpr std::size_t resultSize = 10;
    std::array<std::uint8_t, resultSize> result{};
    int i = 0;
    while (input > 0)
    {
        std::uint64_t rem;
        std::tie(input, rem) = div_rem(input, 58);
        result[resultSize - 1 - i] = rem;
        i += 1;
    }
    assert(i <= 10);

    return result;
}
}  // namespace detail
}  // namespace b58_fast
#endif
