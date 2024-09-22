#include <fmt/base.h>
#include <ostream>
#include <sstream>
#include <string_view>
#include <variant>

#include "twine.hh"

namespace l1 {

class StringTwine final {
private:
  using Child = std::variant<const StringTwine *, std::string_view>;

  Child lhs_;
  Child rhs_;

public:
  explicit StringTwine(const StringTwine &lhs, const StringTwine &rhs)
      : lhs_(&lhs), rhs_(&rhs) {}
  StringTwine(const StringTwine &) = default;
  StringTwine(std::string_view str) : lhs_(str) {}
  StringTwine(const StringTwine &lhs, std::string_view rhs) : lhs_(&lhs), rhs_(rhs) {}
  StringTwine(std::string_view lhs, const StringTwine &rhs) : lhs_(lhs), rhs_(&rhs) {}
  StringTwine(std::string_view lhs, std::string_view rhs)
      : lhs_(lhs), rhs_(rhs) {}

  StringTwine concat(const StringTwine &suffix) const {
    return StringTwine{*this, suffix};
  }

  void print(std::ostream &os) const {
    print(os, lhs_);
    print(os, rhs_);
  }

  std::string str() const {
    std::stringstream ss{};
    print(ss);
    return ss.str();
  }

private:
  static void print(std::ostream &os, const Child &child) {
    if (std::holds_alternative<std::string_view>(child)) {
      auto lhs = std::get<std::string_view>(child);
      os << lhs;
    } else if (std::holds_alternative<const StringTwine *>(child)) {
      auto *lhs = std::get<const StringTwine *>(child);
      lhs->print(os);
    }
  }
};

inline StringTwine operator+(const StringTwine &lhs, const StringTwine &rhs) {
  return lhs.concat(rhs);
}

inline StringTwine operator+(const StringTwine &lhs, std::string_view rhs) {
  return StringTwine(lhs, rhs);
}

inline StringTwine operator+(std::string_view lhs, const StringTwine &rhs) {
  return StringTwine(lhs, rhs);
}

} // namespace l1

int main() {
  using namespace l1;
  using namespace std::literals;

  auto st1 = StringTwine{"Hello, ", "world!"};
  auto st2 = st1 + " It's me"sv;
  fmt::println("{}", st2.str());

  return 0;
}
