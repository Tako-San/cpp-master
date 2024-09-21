#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include "tokenizing.hh"

using namespace l1;
using namespace std::literals;

TEST(Tokenizing, Correct) {
  std::vector<std::pair<std::string, std::vector<Token>>> samples{
      {"char", {Token::eChar, Token::eNonConst}},
      {"char*",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst}},
      {"char * ",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst}},
      {"\tchar\t*",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst}},
      {"\nchar\n*",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst}},
      {"\vchar\v*",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst}},
      {"const char", {Token::eChar, Token::eConst}},
      {"char const", {Token::eChar, Token::eConst}},
      {"char*const",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eConst}},
      {"char *const",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eConst}},
      {"char* const",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eConst}},
      {"char * const",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eConst}},
      {"char**",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst,
        Token::ePtr, Token::eNonConst}},
      {"char **",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst,
        Token::ePtr, Token::eNonConst}},
      {"char* *",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst,
        Token::ePtr, Token::eNonConst}},
      {"char * *",
       {Token::eChar, Token::eNonConst, Token::ePtr, Token::eNonConst,
        Token::ePtr, Token::eNonConst}},

      {"char[]",
       {Token::eChar, Token::eNonConst, Token::eArr, Token::eNonConst}},
      {"char [] ",
       {Token::eChar, Token::eNonConst, Token::eArr, Token::eNonConst}},
      {"\tchar\t[]",
       {Token::eChar, Token::eNonConst, Token::eArr, Token::eNonConst}},
      {"\nchar\n[]",
       {Token::eChar, Token::eNonConst, Token::eArr, Token::eNonConst}},
      {"\vchar\v[]",
       {Token::eChar, Token::eNonConst, Token::eArr, Token::eNonConst}},
      {"const char", {Token::eChar, Token::eConst}},
      {"char const", {Token::eChar, Token::eConst}},
  };

  for (const auto &[type, tokens] : samples)
    EXPECT_EQ(tokens, tokenize(type));
}
using t = const char[];
TEST(Tokenizing, Incorrect) {
  std::vector<std::string> types{
      "constchar",        " constchar ",  "constchar*",      "constchar*const",
      "charconst",        "charconst*",   "charconst*const", "constchar[]",
      "constchar[]const", "charconst",    "charconst[]",     "charconst[]const",
      "char[]const",      "char []const", "char[] const",    "char [] const",
      "char[][]",         "char [][]",    "char[] []",       "char [] []",
  };

  for (const auto &type : types) {
    fmt::println("{}", type);
    ASSERT_THROW(tokenize(type), std::runtime_error);
  }
}
