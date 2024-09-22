#include <memory>
#include <string>
#include <string_view>

#include <range/v3/all.hpp>

#include <fmt/ranges.h>

namespace rng = ranges;
namespace vws = rng::views;

template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename Alloc = std::allocator<CharT>>
class BasicCOWString final {
private:
  using COWStringT = BasicCOWString<CharT, Traits, Alloc>;
  using StringT = std::basic_string<CharT, Traits, Alloc>;
  using StringViewT = std::basic_string_view<CharT, Traits>;

  Alloc allocator_;
  std::shared_ptr<StringT> str_;

  void detach() {
    if (!str_.unique())
      str_ = std::allocate_shared<StringT>(allocator_, *str_);
  }

public:
  template <typename... Args>
  explicit BasicCOWString(Args &&...args)
      : str_(std::allocate_shared<StringT>(allocator_,
                                           std::forward<Args>(args)...)) {}
  BasicCOWString(const COWStringT &str) = default;
  BasicCOWString(COWStringT &&str) = default;

  COWStringT &operator=(const COWStringT &str) = default;
  COWStringT &operator=(COWStringT &&str) = default;

  auto empty() const { return str_->empty(); }
  auto size() const { return str_->size(); }
  auto data() const { return StringViewT{*str_}; }

  auto find(StringViewT s) const { return str_->find(s); }

  CharT getChar(StringT::size_type idx) const { return str_->at(idx); }
  void setChar(StringT::size_type idx, CharT c) {
    detach();
    str_->at(idx) = c;
  }
};

template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename Alloc = std::allocator<CharT>>
class COWTokenizer final {
public:
  using COWStringT = BasicCOWString<CharT, Traits, Alloc>;
  using StringViewT = std::basic_string_view<CharT, Traits>;

private:
  CharT delim_;
  COWStringT str_;
  std::size_t begin_;

public:
  COWTokenizer(const COWStringT &str, CharT delim)
      : delim_(delim), str_(str), begin_(0) {}

  auto get() {
    auto str = str_.data();
    auto begin = str.find_first_not_of(delim_, begin_);
    if (begin == str.npos)
      return StringViewT{};

    auto last = str.find_first_of(delim_, begin);
    if (last != str.npos)
      str.remove_suffix(str.size() - last);

    str.remove_prefix(begin);
    begin_ = last;
    return str;
  }
};

using COWString = BasicCOWString<char>;

int main() {
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
