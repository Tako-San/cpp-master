#include <fmt/ranges.h>

#include "cow.hh"

int main() {
  using namespace l1;
  COWString str0{"  Hello  world!  My name   is     Dio!"};
  COWString str1 = str0;
  str1.setChar(0, '!');
  fmt::println("{}", str1.data());

  auto tokenizer = COWTokenizer{str1, ' '};
  auto tokens = tokenizer.get();
  while (!tokens.empty()) {
    fmt::println("{}", tokens);
    tokens = tokenizer.get();
  }

  fmt::println("{}", str0.data());
  tokenizer = COWTokenizer{str0, ' '};
  tokens = tokenizer.get();
  while (!tokens.empty()) {
    fmt::println("{}", tokens);
    tokens = tokenizer.get();
  }

  fmt::println("{}", str0.find("world"));
  return 0;
}
