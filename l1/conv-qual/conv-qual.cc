#include <cstdio>
#include <vector>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/all.hpp>

#include "conv-qual.hh"
#include "tokenizing.hh"

namespace l1 {
QDecomp QDecomp::GetQCombined(const QDecomp &t1, const QDecomp &t2) {
  auto rp3 = vws::zip_with(
      [](auto p1, auto p2) {
        if (p2 == P::eArr)
          return P::eArr;
        return p1;
      },
      t1.p, t2.p);

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

  if (pos != reverse.end())
    pos = std::next(pos);

  auto cst = rng::subrange(reverse.begin(), pos) |
             vws::transform([](const auto &t) { return std::get<2>(t); });
  auto mut = rng::subrange(pos, reverse.end()) |
             vws::transform([](const auto &) { return CV::eConst; });

  return QDecomp{
      .cv = vws::concat(cst, mut) | vws::reverse | rng::to<std::vector<CV>>,
      .p = rp3 | rng::to<std::vector<P>>,
  };
}

bool testqual(std::string_view sv1, std::string_view sv2) {
  auto tok1 = l1::semanticTransform(l1::tokenize(sv1));
  auto tok2 = l1::semanticTransform(l1::tokenize(sv2));

  auto t1 = l1::QDecomp::Get(tok1);
  auto t2 = l1::QDecomp::Get(tok2);
  auto t3 = l1::QDecomp::GetQCombined(t1, t2);

  return t2.cv == t3.cv && t3.p == t3.p;
}

} // namespace l1