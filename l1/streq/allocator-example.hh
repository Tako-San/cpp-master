#ifndef __L1_STREQ_ALLOCATOR_EXAMPLE_HH__
#define __L1_STREQ_ALLOCATOR_EXAMPLE_HH__

#include <memory>

namespace l1 {

template <typename T> class Allocator : public std::allocator<T> {
public:
  typedef std::size_t size_type;
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

} // namespace l1

#endif // __L1_STREQ_ALLOCATOR_EXAMPLE_HH__
