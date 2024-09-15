#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

#include "tokenizing.hh"

using namespace l1;
using namespace std::literals;

TEST(Tokenizing, Correct) {
  std::vector<std::pair<std::string, std::vector<Token>>> samples{
      {"char", {Token::eChar}},
      {"char*", {Token::eChar, Token::ePtr}},
      {"char * ", {Token::eChar, Token::ePtr}},
      {"\tchar\t*", {Token::eChar, Token::ePtr}},
      {"\nchar\n*", {Token::eChar, Token::ePtr}},
      {"\vchar\v*", {Token::eChar, Token::ePtr}},
      {"const char", {Token::eConst, Token::eChar}},
      {"char const", {Token::eChar, Token::eConst}},
      {"char*const", {Token::eChar, Token::ePtr, Token::eConst}},
      {"char *const", {Token::eChar, Token::ePtr, Token::eConst}},
      {"char* const", {Token::eChar, Token::ePtr, Token::eConst}},
      {"char * const", {Token::eChar, Token::ePtr, Token::eConst}},
      {"char**", {Token::eChar, Token::ePtr, Token::ePtr}},
      {"char **", {Token::eChar, Token::ePtr, Token::ePtr}},
      {"char* *", {Token::eChar, Token::ePtr, Token::ePtr}},
      {"char * *", {Token::eChar, Token::ePtr, Token::ePtr}},

      {"char[]", {Token::eChar, Token::eArr}},
      {"char [] ", {Token::eChar, Token::eArr}},
      {"\tchar\t[]", {Token::eChar, Token::eArr}},
      {"\nchar\n[]", {Token::eChar, Token::eArr}},
      {"\vchar\v[]", {Token::eChar, Token::eArr}},
      {"const char", {Token::eConst, Token::eChar}},
      {"char const", {Token::eChar, Token::eConst}},
      {"char[]const", {Token::eChar, Token::eArr, Token::eConst}},
      {"char []const", {Token::eChar, Token::eArr, Token::eConst}},
      {"char[] const", {Token::eChar, Token::eArr, Token::eConst}},
      {"char [] const", {Token::eChar, Token::eArr, Token::eConst}},
      {"char[][]", {Token::eChar, Token::eArr, Token::eArr}},
      {"char [][]", {Token::eChar, Token::eArr, Token::eArr}},
      {"char[] []", {Token::eChar, Token::eArr, Token::eArr}},
      {"char [] []", {Token::eChar, Token::eArr, Token::eArr}},
  };

  for (const auto &[type, tokens] : samples)
    ASSERT_EQ(tokens, tokenize(type));
}
using t = const char[];
TEST(Tokenizing, Incorrect) {
  std::vector<std::string> types{
      "constchar",        " constchar ", "constchar*",      "constchar*const",
      "charconst",        "charconst*",  "charconst*const", "constchar[]",
      "constchar[]const", "charconst",   "charconst[]",     "charconst[]const",
  };

  for (const auto &type : types)
    ASSERT_THROW(str2tok(type), std::runtime_error);
}
