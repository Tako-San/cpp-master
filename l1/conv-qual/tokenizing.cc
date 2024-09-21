#include <string_view>
#include <vector>

#include "tokenizing.hh"

using namespace std::literals;

namespace l1 {

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
}

template <>
struct fmt::formatter<l1::State> : fmt::formatter<fmt::string_view> {
public:
  template <typename FormatContext>
  auto format(l1::State state, FormatContext &ctx) const {
    fmt::string_view name{};
    switch (state) {
    case l1::State::eInvalid:
      name = "eInvalid";
      break;
    case l1::State::eStart:
      name = "eStart";
      break;
    case l1::State::eFinish:
      name = "eFinish";
      break;
    case l1::State::eChar:
      name = "eChar";
      break;
    case l1::State::eConst0:
      name = "eConst0";
      break;
    case l1::State::eConstChar:
      name = "eConstChar";
      break;
    case l1::State::eArr:
      name = "eArr";
      break;
    case l1::State::ePtr:
      name = "ePtr";
      break;
    case l1::State::ePtrConst:
      name = "ePtrConst";
      break;
    default:
      name = "UNKNOWN";
      break;
    }
    return fmt::formatter<fmt::string_view>::format(name, ctx);
  }
};

namespace l1 {

template <rng::input_range Range> std::vector<Token> str2tok(Range str) {
  auto beg = str.begin();
  auto end = str.end();

  std::string_view w{};
  if (w = "char"; rng::equal(beg, end, w.begin(), w.end()))
    return {Token::eChar};

  std::vector<Token> tokens{};
  if (w = "char"; rng::starts_with(beg, end, w.begin(), w.end())) {
    tokens.push_back(Token::eChar);
    std::advance(beg, w.size());

    if (w = "*"; rng::starts_with(beg, end, w.begin(), w.end())) {
      tokens.push_back(Token::ePtr);
      std::advance(beg, w.size());
    } else if (w = "[]"; rng::starts_with(beg, end, w.begin(), w.end())) {
      tokens.push_back(Token::eArr);
      std::advance(beg, w.size());
    } else {
      throw std::runtime_error{"Unknown token in: " +
                               (str | rng::to<std::string>())};
    }
  }

  for (; beg != end; std::advance(beg, w.size())) {
    if (w = "const"; rng::starts_with(beg, end, w.begin(), w.end())) {
      tokens.push_back(Token::eConst);
    } else if (w = "[]"; rng::starts_with(beg, end, w.begin(), w.end())) {
      tokens.push_back(Token::eArr);
    } else if (w = "*"; rng::starts_with(beg, end, w.begin(), w.end())) {
      tokens.push_back(Token::ePtr);
    } else {
      throw std::runtime_error{"Unknown token in: " +
                               (str | rng::to<std::string>())};
    }
  }

  return tokens;
}

auto tokenizeInner(std::string_view sv) {
  auto tokens =
      vws::transform(sv, [](auto c) { return std::isspace(c) ? ' ' : c; }) |
      vws::split(' ') | vws::filter([](auto w) { return !w.empty(); }) |
      vws::transform([](auto w) { return str2tok(w); }) | vws::join |
      rng::to<std::vector>();
  return tokens;
}

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
    auto cv = tokens.back();
    tokens.push_back(Token::eArr);
    tokens.push_back(cv);
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

std::vector<Token> tokenize(std::string_view sv) {
  auto inputTokens = tokenizeInner(sv);
  const auto table = getTable();

  std::vector<Token> tokens{};
  auto prevState = State::eStart;
  for (auto token : inputTokens) {
    auto prevStateIter = table.find(prevState);
    if (prevStateIter == table.end())
      throw std::runtime_error{fmt::format("Invalid state: {}", prevState)};

    auto &transitions = prevStateIter->second;
    auto nextStateIter = transitions.find(token);
    if (nextStateIter == transitions.end())
      throw std::runtime_error{
          fmt::format("Unknown transition from {} by {}", prevState, token)};

    auto nextState = nextStateIter->second;
    updateState(tokens, prevState, nextState);

    prevState = nextState;
  }

  updateState(tokens, prevState, State::eFinish);
  return tokens;
}

} // namespace l1
