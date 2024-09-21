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

  std::shared_ptr<const StringT> str_;

public:
  template <typename... Args>
  BasicCOWString(Args &&...args)
      : str_(std::make_shared<const StringT>(std::forward<Args>(args)...)) {}
  BasicCOWString(const COWStringT &str) = default;
  BasicCOWString(COWStringT &&str) = default;

  COWStringT &operator=(const COWStringT &str) = default;
  COWStringT &operator=(COWStringT &&str) = default;

  auto empty() const { return str_->empty(); }
  auto size() const { return str_->size(); }
  auto data() const { return StringViewT{*str_}; }
  auto find(StringViewT s) const { return str_->find(s); }
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
  COWString str{"  Hello  world!  My name   is     Dio!"};
  auto tokenizer = COWTokenizer{str, ' '};
  auto tokens = tokenizer.get();
  while (!tokens.empty()) {
    fmt::println("{}", tokens);
    tokens = tokenizer.get();
  }

  fmt::println("{}", str.data());
  fmt::println("{}", str.find("world"));
  return 0;
}
