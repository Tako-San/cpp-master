#include <gtest/gtest.h>
#include <string>

#include "allocator-example.hh"
#include "streq.hh"

TEST(StrEq, Basic) {
  std::string s1 = "string1";
  std::string s2 = "string2";
  EXPECT_FALSE(l1::operator==(s1, s2));

  s2 = "string1";
  EXPECT_TRUE(l1::operator==(s1, s2));
}

TEST(StrEq, DiffAllocators) {
  std::basic_string<char, std::char_traits<char>, std::allocator<char>> s1 = "string1";
  std::basic_string<char, std::char_traits<char>, l1::Allocator<char>> s2 = "string2";
  EXPECT_FALSE(operator==(s1, s2));

  s2 = "string1";
  EXPECT_EQ(s1, s2);
}

