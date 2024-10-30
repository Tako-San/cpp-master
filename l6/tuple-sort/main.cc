#include <cstddef>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

#include <boost/hana.hpp>
#include <boost/hana/fwd/sort.hpp>
#include <boost/hana/fwd/transform.hpp>
#include <boost/hana/fwd/tuple.hpp>
#include <boost/hana/fwd/type.hpp>
#include <boost/hana/fwd/unpack.hpp>

namespace hana = boost::hana;

template <typename T>
concept TupleLike = requires {
  typename std::tuple_size<T>::type;
  typename std::tuple_element<0, T>::type;
  std::get<0>(std::declval<T>());
};

template <TupleLike Tuple, std::size_t... I>
constexpr auto toHana(const Tuple &tuple, std::index_sequence<I...>) {
  return hana::make_tuple(std::make_pair(
      hana::type_c<std::tuple_element<I, Tuple>>, std::get<I>(tuple))...);
}

template <TupleLike Tuple> constexpr auto sort(Tuple t) {
  auto ht = toHana(t, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
  auto sorted = hana::sort(
      ht, [](auto l, auto r) { return hana::sizeof_(l) < hana::sizeof_(r); });

  return hana::unpack(sorted, [](auto... e) { return Tuple{e.second...}; });
}

TEST(SortTuple, StandardTuple) {
  std::tuple<int, char, double> original{1, 'c', 2.0};
  std::tuple<char, int, double> expected{'c', 1, 2.0};
  auto sorted = sort(original);

  ASSERT_EQ(sorted, expected);
}

TEST(SortTuple, StrandardTupleSameSize) {
  std::tuple<int, char, float, double> original{1, 'c', 7.0, 2.0};
  std::tuple<char, int, float, double> expected{'c', 1, 7.0, 2.0};
  auto sorted = sort(original);

  ASSERT_EQ(sorted, expected);
}

TEST(SortTuple, StrandardTupleDuplicate) {
  std::tuple<int, char, int, double> original{1, 'c', 7, 2.0};
  std::tuple<char, int, int, double> expected{'c', 1, 7, 2.0};
  auto sorted = sort(original);

  ASSERT_EQ(sorted, expected);
}

TEST(SortTuple, Pair) {
  std::pair<int, char> original{1, 'c'};
  std::pair<char, int> expected{'c', 1};
  auto sorted = sort(original);

  ASSERT_EQ(sorted, expected);
}

TEST(SortTuple, Array) {
  std::array<int, 3> original{9, 1, 4};
  std::array<int, 3> expected{9, 1, 4};
  auto sorted = sort(original);

  ASSERT_EQ(sorted, expected);
}
