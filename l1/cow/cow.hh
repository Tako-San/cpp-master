#ifndef __L1_COW_COW_HH__
#define __L1_COW_COW_HH__

#include <memory>
#include <string>
#include <string_view>

#include <range/v3/all.hpp>

namespace l1 {

namespace rng = ranges;
namespace vws = rng::views;

template <typename CharT, typename Traits = std::char_traits<CharT>>
class BasicCOWString final {
private:
  using COWStringT = BasicCOWString<CharT, Traits>;
  using StringT = std::basic_string<CharT, Traits>;
  using StringViewT = std::basic_string_view<CharT, Traits>;

  std::shared_ptr<StringT> str_;

public:
  using traits_type = StringT::traits_type;
  using value_type = StringT::value_type;
  using allocator_type = StringT::allocator_type;

  using size_type = StringT::size_type;
  using difference_type = StringT::difference_type;

  using reference = StringT::reference;
  using const_reference = StringT::const_reference;

  using pointer = StringT::pointer;
  using const_pointer = StringT::const_pointer;

private:
  void detach() {
    if (!str_.unique())
      str_ = std::make_shared<StringT>(*str_);
  }

public:
  template <typename... Args>
  explicit BasicCOWString(Args &&...args)
      : str_(std::make_shared<StringT>(std::forward<Args>(args)...)) {}
  BasicCOWString(const COWStringT &str) = default;
  BasicCOWString(COWStringT &&str) = default;

  COWStringT &operator=(const COWStringT &str) = default;
  COWStringT &operator=(COWStringT &&str) = default;

  auto empty() const { return str_->empty(); }
  auto size() const { return str_->size(); }

  const_pointer data() const { return str_->data(); }
  StringT str() const { return *str_; }

  template <typename StringViewLike>
    requires std::is_convertible_v<const StringViewLike &, StringViewT>
  auto find(const StringViewLike &s, size_type pos = 0) const {
    StringViewT sv = s;
    return str_->find(sv, pos);
  }

  CharT getChar(StringT::size_type idx) const { return str_->at(idx); }
  void setChar(StringT::size_type idx, CharT c) {
    detach();
    str_->at(idx) = c;
  }
};

template <typename CharT, typename Traits = std::char_traits<CharT>>
class COWTokenizer final {
public:
  using COWStringT = BasicCOWString<CharT, Traits>;
  using StringViewT = std::basic_string_view<CharT, Traits>;

private:
  CharT delim_;
  COWStringT str_;
  std::size_t begin_;

public:
  COWTokenizer(const COWStringT &str, CharT delim)
      : delim_(delim), str_(str), begin_(0) {}

  auto get() {
    StringViewT str = str_.data();
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

} // namespace l1

#endif // __L1_COW_COW_HH__
