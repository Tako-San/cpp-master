#ifndef __L1_CONV_QUAL_CONV_QUAL_HH__
#define __L1_CONV_QUAL_CONV_QUAL_HH__

#include <fmt/base.h>
#include <fmt/ranges.h>

#include "tokenizing.hh"

namespace l1 {

enum class CV : std::uint8_t { eConst, eNonConst };
enum class P : std::uint8_t { ePtr, eArr };

} // namespace l1

template <> struct fmt::formatter<l1::CV> : fmt::formatter<fmt::string_view> {
public:
  template <typename FormatContext>
  auto format(l1::CV cv, FormatContext &ctx) const {
    fmt::string_view name{};
    switch (cv) {
    case l1::CV::eConst:
      name = "const";
      break;
    case l1::CV::eNonConst:
      name = "non-const";
      break;
    default:
      name = "UNKNOWN";
      break;
    }
    return fmt::formatter<fmt::string_view>::format(name, ctx);
  }
};

template <> struct fmt::formatter<l1::P> : fmt::formatter<fmt::string_view> {
public:
  template <typename FormatContext>
  auto format(l1::P p, FormatContext &ctx) const {
    fmt::string_view name{};
    switch (p) {
    case l1::P::ePtr:
      name = "*";
      break;
    case l1::P::eArr:
      name = "[]";
      break;
    default:
      name = "UNKNOWN";
      break;
    }
    return fmt::formatter<fmt::string_view>::format(name, ctx);
  }
};

namespace l1 {

struct QDecomp final {
  std::vector<CV> cv;
  std::vector<P> p;

  template <rng::range Range> static QDecomp Get(Range range) {
    auto base = range | vws::drop(1) | vws::reverse | vws::drop(1);
    auto rp = base | vws::stride(2) | vws::transform([](auto token) {
                if (token == Token::eArr)
                  return P::eArr;
                else if (token == Token::ePtr)
                  return P::ePtr;
                else
                  throw std::runtime_error{
                      fmt::format("Unexpected token: {}", token)};
              });
    auto rcv =
        base | vws::drop(1) | vws::stride(2) | vws::transform([](auto token) {
          if (token == Token::eConst)
            return CV::eConst;
          else if (token == Token::eNonConst)
            return CV::eNonConst;
          else
            throw std::runtime_error{
                fmt::format("Unexpected token: {}", token)};
        });

    QDecomp res{};
    for (auto [p, cv] : vws::zip(rp, rcv)) {
      res.p.push_back(p);
      res.cv.push_back(cv);
    }

    return res;
  }

  static QDecomp GetQCombined(const QDecomp &t1, const QDecomp &t2);
};

bool testqual(std::string_view sv1, std::string_view sv2);

} // namespace l1

template <> class fmt::formatter<l1::QDecomp> {
public:
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  template <typename FormatContext>
  constexpr auto format(l1::QDecomp const &qd, FormatContext &ctx) const {
    return format_to(ctx.out(), "(cv: {}, p: {})", qd.cv, qd.p);
  }
};

#endif // __L1_CONV_QUAL_CONV_QUAL_HH__
