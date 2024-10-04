#ifndef __L1_TWINE_TWINE_HH__
#define __L1_TWINE_TWINE_HH__

#include <memory>
#include <ostream>
#include <sstream>
#include <string_view>
#include <variant>

namespace l1 {

struct StringTwineNode final {
  using StringTwineNodePtr = std::shared_ptr<StringTwineNode>;
  using StringViewPtr = std::shared_ptr<std::string_view>;
  using Child = std::variant<std::monostate, StringTwineNodePtr, StringViewPtr>;

  Child lhs_;
  Child rhs_;

  void print(std::ostream &os) const {
    print(os, lhs_);
    print(os, rhs_);
  }

  static void print(std::ostream &os, const Child &child) {
    if (std::holds_alternative<StringViewPtr>(child)) {
      auto data = std::get<StringViewPtr>(child);
      os << *data;
    } else if (std::holds_alternative<StringTwineNodePtr>(child)) {
      auto data = std::get<StringTwineNodePtr>(child);
      data->print(os);
    }
  }
};

class StringTwine final {
private:
  std::shared_ptr<StringTwineNode> node_;

public:
  template <typename... Args>
  StringTwine(Args &&...args) : node_(std::make_shared<StringTwineNode>()) {
    concat(std::forward<Args>(args)...);
  }

  template <typename... Args> void concat(std::string_view sv, Args &&...args) {
    concatOne(sv);
    concat(std::forward<Args>(args)...);
  }

  void concat() {}

  void concatOne(std::string_view sv) {
    if (std::holds_alternative<std::monostate>(node_->lhs_)) {
      node_->lhs_ = std::make_shared<std::string_view>(sv.data(), sv.size());
      return;
    }

    if (std::holds_alternative<std::monostate>(node_->rhs_)) {
      node_->rhs_ = std::make_shared<std::string_view>(sv.data(), sv.size());
      return;
    }

    auto node = std::make_shared<StringTwineNode>();
    node->lhs_ = node_;
    node->rhs_ = std::make_shared<std::string_view>(sv.data(), sv.size());
    node_ = node;
  }

  void print(std::ostream &os) const && { node_->print(os); }

  std::string str() const && {
    std::stringstream ss{};
    node_->print(ss);
    return ss.str();
  }
};

} // namespace l1

#endif // __L1_TWINE_TWINE_HH__
