#ifndef __L1_CONV_QUAL_TOKENIZING_HH__
#define __L1_CONV_QUAL_TOKENIZING_HH__

#include <cstdint>
#include <iterator>
#include <string_view>
#include <vector>

#include <fmt/base.h>
#include <fmt/format.h>
#include <range/v3/all.hpp>

namespace l1 {

namespace rng = ranges;
namespace vws = rng::views;

enum class Token : std::uint8_t { eConst, eChar, eArr, ePtr, eNonConst, eEOF };

std::vector<Token> tokenize(std::string_view sv);

} // namespace l1

template <>
struct fmt::formatter<l1::Token> : fmt::formatter<fmt::string_view> {
public:
  template <typename FormatContext>
  auto format(l1::Token token, FormatContext &ctx) const {
    fmt::string_view name{};
    switch (token) {
    case l1::Token::eConst:
      name = "const";
      break;
    case l1::Token::eNonConst:
      name = "non-const";
      break;
    case l1::Token::eChar:
      name = "char";
      break;
    case l1::Token::eArr:
      name = "[]";
      break;
    case l1::Token::ePtr:
      name = "*";
      break;
    case l1::Token::eEOF:
      name = "EOF";
      break;
    default:
      name = "UNKNOWN";
      break;
    }
    return fmt::formatter<fmt::string_view>::format(name, ctx);
  }
};

#endif // __L1_CONV_QUAL_TOKENIZING_HH__
