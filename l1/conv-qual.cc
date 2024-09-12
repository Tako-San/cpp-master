#include <cstdint>
#include <iostream>
#include <list>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <format>

enum class Token : std::uint8_t { eConst, eChar, eArr, ePtr, eEOF };

auto tokenize(std::string_view sv) {
  auto start = sv.find_first_not_of(" \t\n");
  if (start == std::string_view::npos)
    return std::vector<Token>{};
  sv.remove_prefix(start);

  if (sv.contains("constchar") || sv.contains("charconst"))
    throw std::runtime_error{"Error: " + std::string(sv)};

  std::vector<Token> tokens{};

  using namespace std::literals;
  while (!sv.empty()) {
    std::string_view word{};
    if (word = "const"; sv.starts_with(word))
      tokens.push_back(Token::eConst);
    else if (word = "char"; sv.starts_with(word))
      tokens.push_back(Token::eChar);
    else if (word = "[]"; sv.starts_with(word))
      tokens.push_back(Token::eArr);
    else if (word = "*"; sv.starts_with(word))
      tokens.push_back(Token::ePtr);
    else
      throw std::runtime_error{"Unknown token: " + std::string(sv)};

    sv.remove_prefix(word.size());
    auto pos = sv.find_first_not_of(" \t\n");
    if (pos == std::string_view::npos)
      break;
    sv.remove_prefix(pos);
  }

  return tokens;
}

auto tok2str(auto tok) {
  switch (tok) {
  case Token::eConst:
    return "const";
  case Token::eChar:
    return "char";
  case Token::eArr:
    return "[]";
  case Token::ePtr:
    return "*";
  case Token::eEOF:
    return "EOF";
  default:
    throw std::runtime_error{"Unknown token"};
  }
  return "EOF";
};

void print(std::span<Token> tokens) {
  for (auto tok : tokens)
    std::cout << tok2str(tok) << " ";
  std::cout << std::endl;
}

enum class CVQual : std::uint8_t { eConst, eNone };
enum class P : std::uint8_t { ePtr, eArr };

class QDecomp final {
public:
  QDecomp() = default;
  QDecomp(std::span<CVQual> cvq, std::span<P> p)
      : cvq_(cvq.begin(), cvq.end()), p_(p.begin(), p.end()){};

private:
  std::vector<CVQual> cvq_;
  std::vector<P> p_;

public:
  static QDecomp getQualCombinedType(const QDecomp &t1, const QDecomp &t2) {
    if (t1.cvq_.size() != t2.cvq_.size())
      throw std::runtime_error{"cvq sizes are not equal"};
    if (t1.p_.size() != t2.p_.size())
      throw std::runtime_error{"p sizes are not equal"};
    QDecomp t3{};

    return t3;
  }

  static QDecomp getQDecomp(std::span<Token> tokens) {
    if (tokens.empty())
      throw std::runtime_error{"Not enough tokens"};

    std::list<CVQual> cvq;
    if (tokens[0] == Token::eConst)
      cvq.push_front(CVQual::eConst);

    return {};
  }
};

bool testqual(std::string_view from, std::string_view to) {
  /* code */
  return false;
}

int main() try {
  std::string str{"  const   char *const* "};
  auto res = tokenize(str);
  print(res);

  char test[100] = "heloooooooooooooooooooooooo woooooooooooooooorld";
  test[10] = 0;
  std::cout << std::format("{}.", test) << std::endl;

  return 0;
} catch (std::runtime_error &e) {
  std::cerr << e.what() << std::endl;
  return 1;
}
