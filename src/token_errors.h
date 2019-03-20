#include <system_error>

enum class TokenCodecErrc {
    Success = 0,
    InputTooLarge,
    InputTooSmall,
    BadB58Character,
    OutputTooSmall,
    MismatchedTokenType,
    MismatchedChecksum,
    InvalidEncodingChar,
    Unknown,
};

namespace std {
template <>
struct is_error_code_enum<TokenCodecErrc> : true_type
{
};
}  // namespace std

namespace detail {
class TokenCodecErrcCategory : public std::error_category
{
public:
    // Return a short descriptive name for the category
    virtual const char*
    name() const noexcept override final
    {
        return "TokenCodecError";
    }
    // Return what each enum means in text
    virtual std::string
    message(int c) const override final
    {
        switch (static_cast<TokenCodecErrc>(c))
        {
            case TokenCodecErrc::Success:
                return "conversion successful";
            case TokenCodecErrc::InputTooLarge:
                return "input too large";
            case TokenCodecErrc::InputTooSmall:
                return "input too small";
            case TokenCodecErrc::BadB58Character:
                return "bad base 58 character";
            case TokenCodecErrc::OutputTooSmall:
                return "output too small";
            case TokenCodecErrc::MismatchedTokenType:
                return "mismatched token type";
            case TokenCodecErrc::MismatchedChecksum:
                return "mismatched checksum";
            case TokenCodecErrc::InvalidEncodingChar:
                return "invalid encoding char";
            case TokenCodecErrc::Unknown:
                return "unknown";
            default:
                return "unknown";
        }
    }
};
}  // namespace detail

inline const detail::TokenCodecErrcCategory&
TokenCodecErrcCategory()
{
    static detail::TokenCodecErrcCategory c;
    return c;
}

inline std::error_code
make_error_code(TokenCodecErrc e)
{
    return {static_cast<int>(e), TokenCodecErrcCategory()};
}
