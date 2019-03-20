//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#ifndef RIPPLE_PROTOCOL_DIGEST_H_INCLUDED
#define RIPPLE_PROTOCOL_DIGEST_H_INCLUDED

#include <boost/endian/conversion.hpp>

#include <algorithm>
#include <array>

namespace ripple {

/** SHA-256 digest

    @note This uses the OpenSSL implementation
*/
struct openssl_sha256_hasher
{
public:
    static constexpr auto const endian = boost::endian::order::native;

    using result_type = std::array<std::uint8_t, 32>;

    openssl_sha256_hasher();

    void
    operator()(void const* data, std::size_t size) noexcept;

    [[nodiscard]] explicit operator result_type() noexcept;

private:
    char ctx_[112];
};

//------------------------------------------------------------------------------

using sha256_hasher = openssl_sha256_hasher;

}  // namespace ripple

#endif
