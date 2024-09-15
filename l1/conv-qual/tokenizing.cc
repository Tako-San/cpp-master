#include <string_view>
#include <vector>

#include "tokenizing.hh"

using namespace std::literals;

namespace l1 {

std::vector<Token> tokenize(std::string_view sv) {
  auto tokens =
      vws::transform(sv, [](auto c) { return std::isspace(c) ? ' ' : c; }) |
      vws::split(' ') | vws::filter([](auto w) { return !w.empty(); }) |
      vws::transform([](auto w) { return str2tok(w); }) | vws::join |
      rng::to<std::vector>();
  return tokens;
}

enum class State : std::uint8_t {
  eInvalid,
  eStart,
  eFinish,
  eChar,
  eConst0,
  eConstChar,
  eArr,
  ePtr,
  ePtrConst
};

static void updateState(std::vector<Token> &tokens, State from, State to) {
  if (from == State::eConstChar &&
      (to == State::eFinish || to == State::eFinish || to == State::eArr ||
       to == State::ePtr)) {
    tokens.push_back(Token::eChar);
    tokens.push_back(Token::eConst);
  } else if (from == State::eChar &&
             (to == State::eArr || to == State::ePtr || to == State::eFinish)) {
    tokens.push_back(Token::eChar);
    tokens.push_back(Token::eNonConst);
  } else if (from == State::eArr && to == State::eFinish) {
    tokens.push_back(Token::eArr);
    tokens.push_back(Token::eConst);
  } else if (from == State::ePtr &&
             (to == State::ePtr || to == State::eArr || to == State::eFinish)) {
    tokens.push_back(Token::ePtr);
    tokens.push_back(Token::eNonConst);
  } else if (from == State::ePtrConst &&
             (to == State::ePtr || to == State::eArr || to == State::eFinish)) {
    tokens.push_back(Token::ePtr);
    tokens.push_back(Token::eConst);
  }
}

static auto getTable() {
  std::unordered_map<State, std::unordered_map<Token, State>> table{};
  table[State::eStart] = {{Token::eChar, State::eChar},
                          {Token::eConst, State::eConst0}};
  table[State::eConst0] = {{Token::eChar, State::eConstChar}};
  table[State::eChar] = {{Token::eArr, State::eArr},
                         {Token::ePtr, State::ePtr},
                         {Token::eConst, State::eConstChar},
                         {Token::eEOF, State::eFinish}};
  table[State::eConstChar] = {{Token::eArr, State::eArr},
                              {Token::ePtr, State::ePtr},
                              {Token::eEOF, State::eFinish}};
  table[State::ePtr] = {{Token::eConst, State::ePtrConst},
                        {Token::ePtr, State::ePtr},
                        {Token::eArr, State::eArr},
                        {Token::eEOF, State::eFinish}};
  table[State::ePtrConst] = {{Token::ePtr, State::ePtr},
                             {Token::eArr, State::eArr},
                             {Token::eEOF, State::eFinish}};
  table[State::eArr] = {{Token::eEOF, State::eFinish}};
  return table;
}

std::vector<Token> semanticTransform(std::vector<Token> inputTokens) {
  const auto table = getTable();

  std::vector<Token> tokens{};
  auto prevState = State::eStart;
  for (auto token : inputTokens) {
    auto prevStateIter = table.find(prevState);
    if (prevStateIter == table.end())
      throw std::runtime_error{"Invalid state"};

    auto &transitions = prevStateIter->second;
    auto nextStateIter = transitions.find(token);
    if (nextStateIter == transitions.end())
      throw std::runtime_error{"Unknown transition"};

    auto nextState = nextStateIter->second;
    updateState(tokens, prevState, nextState);

    prevState = nextState;
  }

  updateState(tokens, prevState, State::eFinish);
  return tokens;
}

} // namespace l1
