#include <format>
#include <iostream>
#include <memory>
#include <string>

template <typename CharT, typename Traits, typename AllocLHS, typename AllocRHS>
bool operator==(const std::basic_string<CharT, Traits, AllocLHS> &lhs,
                const std::basic_string<CharT, Traits, AllocRHS> &rhs) {
  if (lhs.size() != rhs.size())
    return false;
  return !Traits::compare(lhs.data(), rhs.data(), lhs.size());
}

template <typename CharT, typename Traits, typename AllocLHS>
bool operator==(const std::basic_string<CharT, Traits, AllocLHS> &lhs,
                const char *rhs) {
  return !lhs.compare(rhs);
}

template <typename CharT, typename Traits, typename AllocRHS>
bool operator==(const char *lhs,
                const std::basic_string<CharT, Traits, AllocRHS> &rhs) {
  return !rhs.compare(lhs);
}

template <typename T> class Allocator : public std::allocator<T> {
public:
  typedef size_t size_type;
  typedef T *pointer;
  typedef const T *const_pointer;

  template <typename _Tp1> struct rebind {
    typedef Allocator<_Tp1> other;
  };

  pointer allocate(size_type n) { return std::allocator<T>::allocate(n); }

  void deallocate(pointer p, size_type n) {
    return std::allocator<T>::deallocate(p, n);
  }

  Allocator() throw() : std::allocator<T>() {}
  Allocator(const Allocator &a) throw() : std::allocator<T>(a) {}
  template <class U>
  Allocator(const Allocator<U> &a) throw() : std::allocator<T>(a) {}
  ~Allocator() throw() {}
};

int main() {
  std::string lhs = "str1";
  std::basic_string<char, std::char_traits<char>, Allocator<char>> rhs = "str1";

  std::cout << std::format("{} is {}equal to {}", lhs,
                           (lhs == rhs) ? "" : "not ", rhs)
            << std::endl;

#if 0
  std::cout << std::format("{} is {}equal to {}", lhs,
                           (lhs == "word") ? "" : "not ", rhs)
            << std::endl;
#endif

  return 0;
}
