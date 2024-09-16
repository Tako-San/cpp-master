#include <cstdio>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/all.hpp>

#include "tokenizing.hh"

namespace l1 {

enum class CV : std::uint8_t { eConst, eNonConst };
enum class P : std::uint8_t { ePtr, eArr };

struct QDecomp final {
  std::vector<CV> cv;
  std::vector<P> p;
  CV cv0;

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

    auto cv0 = range.back();
    if (cv0 == Token::eConst)
      res.cv0 = CV::eConst;
    else if (cv0 == Token::eNonConst)
      res.cv0 = CV::eNonConst;
    else
      throw std::runtime_error{"Unexpected token"};

    return res;
  }

  static QDecomp GetQCombined(const QDecomp &t1, const QDecomp &t2) {
    auto rp3 = vws::zip_with(
        [](auto p1, auto p2) { return (p2 == P::eArr) ? P::eArr : p1; }, t1.p,
        t2.p);

    auto rcv3 = vws::zip_with(
        [](auto cv1, auto cv2) {
          if (cv1 == CV::eConst)
            return CV::eConst;
          if (cv2 == CV::eConst)
            return CV::eConst;
          return CV::eNonConst;
        },
        t1.cv, t2.cv);

    auto reverse = vws::zip(t1.cv, t2.cv, rcv3, t1.p, t2.p, rp3) | vws::reverse;
    auto pos = rng::find_if(reverse, [](const auto &t) {
      const auto &[cv1, cv2, cv3, p1, p2, p3] = t;
      return (cv3 != cv1) || (cv3 != cv2) || (p3 != p1) || (p3 != p2);
    });

    pos = std::next(pos);

    auto cst = rng::subrange(reverse.begin(), pos) |
               vws::transform([](const auto &t) { return std::get<2>(t); });
    auto mut = rng::subrange(pos, reverse.end()) |
               vws::transform([](const auto &) { return CV::eConst; });

    return QDecomp{
        .cv = vws::concat(cst, mut) | vws::reverse | rng::to<std::vector<CV>>,
        .p = rp3 | rng::to<std::vector<P>>,
        .cv0 = t1.cv0,
    };
  }
};

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

template <> class fmt::formatter<l1::QDecomp> {
public:
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
  template <typename FormatContext>
  constexpr auto format(l1::QDecomp const &qd, FormatContext &ctx) const {
    return format_to(ctx.out(), "(cv0: {}, cv: {}, p: {})", qd.cv0, qd.cv,
                     qd.p);
  }
};

bool testqual(std::string_view sv1, std::string_view sv2) {
  auto tok1 = l1::semanticTransform(l1::tokenize(sv1));
  auto tok2 = l1::semanticTransform(l1::tokenize(sv2));

  auto t1 = l1::QDecomp::Get(tok1);
  auto t2 = l1::QDecomp::Get(tok2);
  auto t3 = l1::QDecomp::GetQCombined(t1, t2);

  return t2.cv == t3.cv && t3.p == t3.p;
}

int main() try {
  using namespace l1;
  std::string str1{"const char**"};
  std::string str2{"char **"};

  auto tok1 = l1::semanticTransform(l1::tokenize(str1));
  auto tok2 = l1::semanticTransform(l1::tokenize(str2));

  fmt::println("t1: {}", tok1);
  fmt::println("t2: {}", tok2);

  auto t1 = QDecomp::Get(tok1);
  auto t2 = QDecomp::Get(tok2);
  auto t3 = QDecomp::GetQCombined(t1, t2);

  fmt::println("t1: {}", t1);
  fmt::println("t2: {}", t2);
  fmt::println("t3: {}", t3);

  fmt::println("{}", testqual(str1, str2));

  return 0;
} catch (std::runtime_error &e) {
  fmt::println(stderr, "{}", e.what());
  return 1;
}
