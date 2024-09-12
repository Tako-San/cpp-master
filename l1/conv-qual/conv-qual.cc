#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <list>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

enum class Token : std::uint8_t { eConst, eNonConst, eChar, eArr, ePtr };

auto tokenize(std::string_view sv) {
  auto start = sv.find_first_not_of(" \t\n");
  if (start == std::string_view::npos)
    return std::deque<Token>{};
  sv.remove_prefix(start);

  if (sv.contains("constchar") || sv.contains("charconst"))
    throw std::runtime_error{"Error: " + std::string(sv)};

  std::deque<Token> tokens{};

  using namespace std::literals;
  std::string_view word{};
  while (!sv.empty()) {
    if (word = "const"; sv.starts_with(word)) {
      if (!tokens.empty())
        tokens.back() = Token::eConst;
      else
        tokens.push_back(Token::eConst);
    } else if (word = "char"; sv.starts_with(word)) {
      tokens.push_back(Token::eChar);
    } else if (word = "[]"; sv.starts_with(word)) {
      tokens.push_back(Token::eArr);
      tokens.push_back(Token::eNonConst);
    } else if (word = "*"; sv.starts_with(word)) {
      tokens.push_back(Token::ePtr);
      tokens.push_back(Token::eNonConst);
    } else
      throw std::runtime_error{"Unknown token: " + std::string(sv)};

    sv.remove_prefix(word.size());
    auto pos = sv.find_first_not_of(" \t\n");
    if (pos == std::string_view::npos)
      break;
    sv.remove_prefix(pos);
  }

  if (tokens[0] == Token::eConst) { // TODO: check tokens.size()
    std::swap(tokens[0], tokens[1]);
  } else if (tokens[1] != Token::eConst) {
    tokens.push_front(Token::eNonConst);
    std::swap(tokens[0], tokens[1]);
  }

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
  return "EOF";
};

template <typename Iter> void print(Iter from, Iter to) {
  std::for_each(from, to, [](auto tok) { std::cout << tok2str(tok) << " "; });
  std::cout << std::endl;
}

enum class CVQual : std::uint8_t { eConst, eNonConst };
enum class P : std::uint8_t { ePtr, eArr };

class QDecomp final {
public:
  QDecomp() = default;
  QDecomp(std::span<CVQual> cvq, std::span<P> p)
      : cvq_(cvq.begin(), cvq.end()), p_(p.begin(), p.end()){};

private:
  std::vector<CVQual> cvq_{};
  std::vector<P> p_{};

public:
  const auto &cvq() const { return cvq_; }
  const auto &p() const { return p_; }

  static QDecomp getQualCombinedType(const QDecomp &t1, const QDecomp &t2) {
    if (t1.cvq_.size() != t2.cvq_.size())
      throw std::runtime_error{"cvq sizes are not equal"};
    if (t1.p_.size() != t2.p_.size())
      throw std::runtime_error{"p sizes are not equal"};
    auto psize = t1.p_.size();
    auto cvqsize = t1.cvq_.size();
    QDecomp t3{};

    for (std::size_t i = 0; i < psize; ++i) {
      t3.p_.push_back((t1.p_[i] == P::eArr || t2.p_[i] == P::eArr) ? P::eArr
                                                                   : t1.p_[i]);
    }

    for (std::size_t i = 0; i < cvqsize; ++i) {
      t3.cvq_.push_back(
          (t1.cvq_[i] == CVQual::eConst || t2.cvq_[i] == CVQual::eConst)
              ? CVQual::eConst
              : CVQual::eNonConst);
    }

    for (std::size_t i = 0; i < cvqsize; ++i)
      if (t3.cvq_[i] != t1.cvq_[i] || t3.cvq_[i] != t2.cvq_[i])
        for (std::size_t k = 1; k < i; ++k)
          t3.cvq_[k] = CVQual::eConst;

    for (std::size_t i = 0; i < psize; ++i)
      if (t3.p_[i] != t1.p_[i] || t3.p_[i] != t2.p_[i])
        for (std::size_t k = 1; k < i; ++k)
          t3.cvq_[k] = CVQual::eConst;

    return t3;
  }

  template <typename Iter> static QDecomp getQDecomp(Iter from, Iter to) {
    if (to == from)
      throw std::runtime_error{"Not enough tokens"};

    std::list<P> p;
    std::list<CVQual> cvq;
    for (auto cur = from; cur < to; cur += 2) {
      if (*cur == Token::eArr) {
        p.push_front(P::eArr);
      } else if (*cur == Token::ePtr) {
        p.push_front(P::ePtr);
      }

      if (cur[1] == Token::eConst) {
        cvq.push_front(CVQual::eConst);
      } else if (cur[1] == Token::eNonConst) {
        cvq.push_front(CVQual::eNonConst);
      }
    }

    std::vector<P> pvec{p.begin(), p.end()};
    std::vector<CVQual> cvqvec{cvq.begin(), cvq.end()};
    return QDecomp{cvqvec, pvec};
  }
};

bool testqual(std::string_view from, std::string_view to) {
  /* code */
  return false;
}

int main() try {
  std::string str1{"  const  char**"};
  std::string str2{"    char*    *  "};
  auto tok1 = tokenize(str1);
  auto tok2 = tokenize(str2);

  print(tok1.begin(), tok1.end());
  print(tok2.begin(), tok2.end());

  auto t1 = QDecomp::getQDecomp(tok1.begin(), tok1.end());
  auto t2 = QDecomp::getQDecomp(tok2.begin(), tok2.end());
  auto t3 = QDecomp::getQualCombinedType(t1, t2);

  auto p = t1.p();
  auto cvq = t1.cvq();

  std::cout << std::endl << "t1" << std::endl;
  for (auto ep : p)
    std::cout << (int)ep << std::endl;
  std::cout << std::endl;
  for (auto ecvq : cvq)
    std::cout << (int)ecvq << std::endl;

  p = t2.p();
  cvq = t2.cvq();

  std::cout << std::endl << "t2" << std::endl;
  for (auto ep : p)
    std::cout << (int)ep << std::endl;
  std::cout << std::endl;
  for (auto ecvq : cvq)
    std::cout << (int)ecvq << std::endl;

  p = t3.p();
  cvq = t3.cvq();

  std::cout << std::endl << "t3" << std::endl;
  for (auto ep : p)
    std::cout << (int)ep << std::endl;
  std::cout << std::endl;
  for (auto ecvq : cvq)
    std::cout << (int)ecvq << std::endl;



  // char test[100] = "heloooooooooooooooooooooooo woooooooooooooooorld";
  // test[10] = 0;
  // std::cout << std::format("{}.", test) << std::endl;

  return 0;
} catch (std::runtime_error &e) {
  std::cerr << e.what() << std::endl;
  return 1;
}
