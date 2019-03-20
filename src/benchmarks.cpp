#include <benchmark/benchmark.h>

#include "test_utils.h"
#include "tokens.h"

#include <array>
#include <cstddef>
#include <random>
#include <span>

static void
BM_ref_encode(benchmark::State& state)
{
    constexpr std::size_t numToEncode = 256;
    std::array<std::array<std::uint8_t, numToEncode>, numToEncode> b256DataBufs;
    std::array<std::tuple<ripple::TokenType, std::span<uint8_t>>, 128> toEncode;
    for (int i = 0; i < numToEncode; ++i)
    {
        toEncode[i] = random_b256_test_data(
            std::span(b256DataBufs[i].data(), b256DataBufs[i].size()));
    }
    for (auto _ : state)
    {
        for (int i = 0; i < numToEncode; ++i)
        {
            auto& [tokType, span] = toEncode[i];
            auto s = ripple::b58_ref::encodeBase58Token(
                tokType, span.data(), span.size());
            benchmark::DoNotOptimize(s);
        }
    }
}
BENCHMARK(BM_ref_encode);

#ifndef _MSC_VER
static void
BM_encode_non_ms(benchmark::State& state)
{
    constexpr std::size_t numToEncode = 256;
    std::array<std::array<std::uint8_t, numToEncode>, numToEncode> b256DataBufs;
    std::array<std::tuple<ripple::TokenType, std::span<uint8_t>>, 128> toEncode;
    for (int i = 0; i < numToEncode; ++i)
    {
        toEncode[i] = random_b256_test_data(
            std::span(b256DataBufs[i].data(), b256DataBufs[i].size()));
    }
    for (auto _ : state)
    {
        for (int i = 0; i < numToEncode; ++i)
        {
            auto& [tokType, span] = toEncode[i];
            auto s = ripple::b58_fast::encodeBase58Token(
                tokType, span.data(), span.size());
            benchmark::DoNotOptimize(s);
        }
    }
}
BENCHMARK(BM_encode_non_ms);

static void
BM_new_encode(benchmark::State& state)
{
    constexpr std::size_t numToEncode = 256;
    std::array<std::array<std::uint8_t, numToEncode>, numToEncode> b256DataBufs;
    std::array<std::tuple<ripple::TokenType, std::span<uint8_t>>, numToEncode>
        toEncode;
    std::array<std::uint8_t, 128> outBuf{};
    std::span<std::uint8_t> outSpan(outBuf.data(), outBuf.size());
    for (int i = 0; i < numToEncode; ++i)
    {
        toEncode[i] = random_b256_test_data(
            std::span(b256DataBufs[i].data(), b256DataBufs[i].size()));
    }
    for (auto _ : state)
    {
        for (int i = 0; i < numToEncode; ++i)
        {
            auto& [tokType, inSpan] = toEncode[i];
            auto r =
                ripple::b58_fast::encodeBase58Token(tokType, inSpan, outSpan);
            benchmark::DoNotOptimize(r);
        }
    }
}
BENCHMARK(BM_new_encode);
#endif

static void
BM_ref_decode(benchmark::State& state)
{
    constexpr std::size_t numToDecode = 256;
    std::array<std::tuple<ripple::TokenType, std::string>, numToDecode>
        toDecode;
    for (int i = 0; i < numToDecode; ++i)
    {
        std::array<std::uint8_t, numToDecode> b256DataBuf;
        auto [tokType, span] = random_b256_test_data(
            std::span(b256DataBuf.data(), b256DataBuf.size()));
        auto s = ripple::b58_ref::encodeBase58Token(
            tokType, span.data(), span.size());
        toDecode[i] = std::tie(tokType, s);
    }
    for (auto _ : state)
    {
        for (int i = 0; i < numToDecode; ++i)
        {
            auto& [tokType, s] = toDecode[i];
            auto r = ripple::b58_ref::decodeBase58Token(s, tokType);
            benchmark::DoNotOptimize(r);
        }
    }
}
BENCHMARK(BM_ref_decode);

#ifndef _MSC_VER
static void
BM_decode_non_ms(benchmark::State& state)
{
    constexpr std::size_t numToDecode = 256;
    std::array<std::tuple<ripple::TokenType, std::string>, numToDecode>
        toDecode;
    for (int i = 0; i < numToDecode; ++i)
    {
        std::array<std::uint8_t, numToDecode> b256DataBuf;
        auto [tokType, span] = random_b256_test_data(
            std::span(b256DataBuf.data(), b256DataBuf.size()));
        auto s = ripple::encodeBase58Token(tokType, span.data(), span.size());
        toDecode[i] = std::tie(tokType, s);
    }
    for (auto _ : state)
    {
        for (int i = 0; i < numToDecode; ++i)
        {
            auto& [tokType, s] = toDecode[i];
            auto r = ripple::b58_fast::decodeBase58Token(s, tokType);
            benchmark::DoNotOptimize(r);
        }
    }
}
BENCHMARK(BM_decode_non_ms);

static void
BM_new_decode(benchmark::State& state)
{
    constexpr std::size_t numToDecode = 256;
    std::array<std::tuple<ripple::TokenType, std::string>, numToDecode>
        toDecode;
    std::array<std::uint8_t, 128> outBuf{};
    std::span<std::uint8_t> outSpan(outBuf.data(), outBuf.size());
    for (int i = 0; i < numToDecode; ++i)
    {
        std::array<std::uint8_t, 128> b256DataBuf;
        auto [tokType, span] = random_b256_test_data(
            std::span(b256DataBuf.data(), b256DataBuf.size()));
        auto s = ripple::encodeBase58Token(tokType, span.data(), span.size());
        toDecode[i] = std::tie(tokType, s);
    }
    for (auto _ : state)
    {
        for (int i = 0; i < numToDecode; ++i)
        {
            auto& [tokType, s] = toDecode[i];
            auto r = ripple::b58_fast::decodeBase58Token(tokType, s, outSpan);
            benchmark::DoNotOptimize(r);
        }
    }
}
BENCHMARK(BM_new_decode);
#endif

BENCHMARK_MAIN();
