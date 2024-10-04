#include <fmt/base.h>

#include "twine.hh"

int main() {
  using namespace l1;
  using namespace std::literals;

  auto s = "I am test..."s;
  auto st1 = StringTwine{"Hello, ", "world!", " ", s, " Still test."sv}.str();
  fmt::println("{}", st1);

  return 0;
}
