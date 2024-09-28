#include <type_traits>

#include <gtest/gtest.h>

#include "conv-qual.hh"

#define CQ_CHECK(t1, t2)                                                       \
  EXPECT_NE(l1::testqual(#t1, #t2), (std::is_convertible_v<t1, t2>))

template <typename T> void foo(T) {}

void bar0(char *arr[]) { foo<char *const[]>(arr); }
void bar1(char **const arr) { foo<char *[]>(arr); }
void bar2(char **arr) { foo<char *const[]>(arr); }

TEST(ConvQual, Weird) {
  /* IDK why std::is_convertible says false in this cases
     but it works fine for me. See bar* functions. */
  CQ_CHECK(char *[], char *const[]);
  CQ_CHECK(char **, char *const[]);
  CQ_CHECK(char **const, char *[]);
}

#undef CQ_CHECK

#define CQ_CHECK(t1, t2)                                                       \
  EXPECT_EQ(l1::testqual(#t1, #t2), (std::is_convertible_v<t1, t2>))

TEST(ConvQual, Test) {
  CQ_CHECK(char[], char *);

  CQ_CHECK(char const **, char **);
  CQ_CHECK(char **, char const **);
  CQ_CHECK(char *const *, char **);
  CQ_CHECK(char **, char *const *);
  CQ_CHECK(char **const, char **);
  CQ_CHECK(char **, char **const);

  CQ_CHECK(char const *[], char *[]);
  CQ_CHECK(char *[], char const *[]);
  CQ_CHECK(char *const[], char *[]);

  CQ_CHECK(char const *[], char **);
  CQ_CHECK(char *[], char const **);
  CQ_CHECK(char *const[], char **);
  CQ_CHECK(char *[], char *const *);
  CQ_CHECK(char *[], char **const);

  CQ_CHECK(char const **, char *[]);
  CQ_CHECK(char **, char const *[]);
  CQ_CHECK(char *const *, char *[]);
}
