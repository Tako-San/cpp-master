#include <algorithm>
#include <cctype>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <vector>

using namespace std::literals;

namespace rng = std::ranges;
namespace views = rng::views;

namespace l1 {

enum class Token : std::uint8_t { eConst, eNonConst, eChar, eArr, ePtr };

template <rng::input_range Range> auto str2tok(Range str) {
  auto beg = str.begin();
  auto end = str.end();
  std::string_view w{};

  std::vector<Token> tokens{};
  for (auto cur = beg; cur != end; std::advance(cur, w.size())) {
    if (w = "constchar";
        rng::equal(cur, std::next(cur, w.size()), w.begin(), w.end()))
      throw std::runtime_error{"Bad token: constchar"};
    else if (w = "charconst";
             rng::equal(cur, std::next(cur, w.size()), w.begin(), w.end()))
      throw std::runtime_error{"Bad token: charconst"};
    else if (w = "const";
             rng::equal(cur, std::next(cur, w.size()), w.begin(), w.end()))
      tokens.push_back(Token::eConst);
    else if (w = "char";
             rng::equal(cur, std::next(cur, w.size()), w.begin(), w.end()))
      tokens.push_back(Token::eChar);
    else if (w = "[]";
             rng::equal(cur, std::next(cur, w.size()), w.begin(), w.end()))
      tokens.push_back(Token::eArr);
    else if (w = "*";
             rng::equal(cur, std::next(cur, w.size()), w.begin(), w.end()))
      tokens.push_back(Token::ePtr);
    else {
      std::string tok{str.begin(), str.end()};
      throw std::runtime_error{"Unknow token: " + tok};
    }
  }

  return tokens;
}

auto tokenize(std::string_view sv) {
  auto view =
      views::transform(sv, [](auto c) { return std::isspace(c) ? ' ' : c; }) |
      views::split(' ') | views::filter([](auto w) { return !w.empty(); }) |
      views::transform([](auto w) { return str2tok(w); }) | views::join;

  std::vector<Token> tokens{};
  rng::copy(view, std::back_inserter(tokens));
  return tokens;
}

auto tok2str(auto tok) {
  switch (tok) {
  case Token::eConst:
    return "const";
  case Token::eNonConst:
    return "non-const";
  case Token::eChar:
    return "char";
  case Token::eArr:
    return "[]";
  case Token::ePtr:
    return "*";
  default:
    throw std::runtime_error{"Unknown token"};
  }
};

template <rng::input_range Range> void print(Range tokens) {
  rng::for_each(tokens, [](auto tok) { std::cout << tok2str(tok) << " "; });
  std::cout << std::endl;
}

} // namespace l1
