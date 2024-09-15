#include <cstdio>
#include <stdexcept>
#include <vector>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <range/v3/all.hpp>

#include "tokenizing.hh"

int main() try {
  std::string str1{"  const char**[][]"};
  std::string str2{"    char*    *  "};

  auto tok1 = l1::tokenize(str1);
  fmt::println("{}", tok1);
  fmt::println("{}", semanticTransform(tok1));
  return 0;
} catch (std::runtime_error &e) {
  fmt::println(stderr, "{}", e.what());
  return 1;
}
