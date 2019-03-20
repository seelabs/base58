Fast implementations of base 58 encoding and decoding for the XRP ledger.

The new implementations are about 10x faster when encoding and about 15x faster
when decoding. The trick is to use base 58^10 as an intermediate step when
encoding and decoding. Instead of directly encoding from base 2^256 to base 58,
the algorithm first converts from base 2^256 to base 58^10, and then from
base58^10 to base 58. Similarly, when decoding, instead of directly decoding
from base 58 to base 2^256, it first converts from base 58 to base 58^10, and
then from base 58^10 to base 58.

The intermediate base (58^10) was chosen as the largest power of 58 that still
fits in a 64-bit unsigned integer. Computing with 64-bit integers instead of
8-bit integers parallelizes many computations.

A gcc extension was use - 128 bit integers. This is used to multiply two 64-bit
integers and get the full 128 bit result. This can be done with one Intel
instruction, but it is not available in C++ directly. Alternatively, we can
inline assembly instructions for the same effect. Visual studio does not have
this extension, and will fall back to using the slower reference implementation.

It is compiled in C++-20 mode. This is so the `std::span` could be used on the
interface. However, it would be easy to convert this to C++-17
