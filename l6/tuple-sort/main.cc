#include <tuple>
#include <type_traits>

#include <boost/hana.hpp>
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/fwd/sort.hpp>
#include <boost/hana/fwd/transform.hpp>
#include <boost/hana/fwd/tuple.hpp>

namespace hana = boost::hana;

template <typename Tuple> consteval auto sort(Tuple t) {
  auto types = hana::transform(
      t, [](auto e) { return std::type_identity<decltype(e)>{}; });

  return hana::sort(t, [](auto l, auto r){
    return hana::sizeof_(l) < hana::sizeof_(r);
  });
}

int main() {
  constexpr std::tuple<int, char, double> t{1, 'c', 2.0};
  constexpr auto sorted = sort(t);

  static_assert(sorted == std::tuple<char, int, double>{'c', 1, 2.0}, "hi");
}
