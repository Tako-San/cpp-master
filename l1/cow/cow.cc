#include <iostream>
#include <memory>
#include <string>

template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename Alloc = std::allocator<CharT>>
class BasicCOWString final {
private:
  using COWStringT = BasicCOWString<CharT, Traits, Alloc>;
  using StringT = std::basic_string<CharT, Traits, Alloc>;

  std::shared_ptr<StringT> str_;

  class Proxy final {
    // TODO: think
  };

public:
  template <typename... Args>
  BasicCOWString(Args &&...args)
      : str_(std::make_shared<StringT>(std::forward<Args>(args)...)) {}
  BasicCOWString(const COWStringT &str) = default;
  BasicCOWString(COWStringT &&str) = default;

  COWStringT &operator=(const COWStringT &str) = default;
  COWStringT &operator=(COWStringT &&str) = default;

  bool empty() const { return str_->empty(); }
};

using COWString = BasicCOWString<char>;

int main() {
  COWString str{"Hi"};
  std::cout << str.empty() << std::endl;
}