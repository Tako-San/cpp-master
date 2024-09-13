#ifndef __L1_STREQ_STREQ_TEST_HH__
#define __L1_STREQ_STREQ_TEST_HH__

#include <string>

namespace l1 {

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

} // namespace l1

#endif // __L1_STREQ_STREQ_TEST_HH__
