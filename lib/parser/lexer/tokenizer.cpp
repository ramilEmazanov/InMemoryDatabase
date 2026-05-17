#include "tokenizer.hpp"

#include <optional>
#include <variant>
#include <vector>

#include "errors.hpp"
#include "overload.hpp"
#include "token.hpp"

namespace {

struct InitialState {};

struct BarewordState {
  std::string lexeme;
  std::size_t start_column;
};

struct QuotedState {
  std::string lexeme;
  std::size_t start_column;
};

struct EscapeState {
  std::string lexeme;
  std::size_t start_column;
};

struct AcceptState {};

struct ErrorState {
  LexicalError error;
};

using DfaState = std::variant<InitialState, BarewordState, QuotedState,
                              EscapeState, AcceptState, ErrorState>;

DfaState Transition(DfaState state, char ch, std::size_t column,
                    std::vector<Token>& out) {
  return std::visit(
      Overload{
          [&](InitialState) -> DfaState {
            if (ch == ' ' || ch == '\t') {
              return InitialState{};
            }
            if (ch == '"') {
              return QuotedState{"", column};
            }
            if (std::iscntrl(ch)) {
              return ErrorState{UnexpectedControlCharacter{column, ch}};
            }
            return BarewordState{std::string(1, ch), column};
          },
          [&](BarewordState s) -> DfaState {
            if (ch == ' ' || ch == '\t') {
              out.push_back(Token{TokenKind::Bareword, std::move(s.lexeme)});
              return InitialState{};
            }
            if (ch == '"') {
              out.push_back(Token{TokenKind::Bareword, std::move(s.lexeme)});
              return QuotedState{"", column};
            }
            s.lexeme += ch;
            return s;
          },
          [&](QuotedState s) -> DfaState {
            if (ch == '"') {
              out.push_back(Token{TokenKind::Quoted, std::move(s.lexeme)});
              return InitialState{};
            }
            if (ch == '\\')
              return EscapeState{std::move(s.lexeme), s.start_column};
            s.lexeme += ch;
            return s;
          },
          [&](EscapeState s) -> DfaState {
            char decoded;
            switch (ch) {
              case '"':
                decoded = '"';
                break;
              case '\\':
                decoded = '\\';
                break;
              case 'n':
                decoded = '\n';
                break;
              case 't':
                decoded = '\t';
                break;
              default:
                return ErrorState{InvalidEscapeSequence{column, ch}};
            }
            s.lexeme += decoded;
            return QuotedState{std::move(s.lexeme), s.start_column};
          },
          [](AcceptState s) -> DfaState { return s; },
          [](ErrorState s) -> DfaState { return s; },
      },
      state);
}

std::optional<LexicalError> Finalize(DfaState& state, std::vector<Token>& out) {
  return std::visit(
      Overload{
          [](InitialState) -> std::optional<LexicalError> {
            return std::nullopt;
          },
          [&](BarewordState& s) -> std::optional<LexicalError> {
            out.push_back(Token{TokenKind::Bareword, std::move(s.lexeme)});
            return std::nullopt;
          },
          [](QuotedState& s) -> std::optional<LexicalError> {
            return UnterminatedString{s.start_column};
          },
          [](EscapeState& s) -> std::optional<LexicalError> {
            return UnterminatedString{s.start_column};
          },
          [](AcceptState) -> std::optional<LexicalError> {
            return std::nullopt;
          },
          [](ErrorState& s) -> std::optional<LexicalError> { return s.error; },
      },
      state);
}

}  // namespace

std::expected<std::vector<Token>, LexicalError> Tokenizer::Tokenize(
    std::string_view line) {
  std::vector<Token> tokens;

  DfaState state = InitialState{};

  for (std::size_t i = 0; i < line.size(); ++i) {
    state = Transition(state, line[i], i, tokens);

    if (std::holds_alternative<ErrorState>(state)) {
      return std::unexpected{std::get<ErrorState>(state).error};
    }
  }

  auto res = Finalize(state, tokens);

  if (res.has_value()) {
    return std::unexpected{res.value()};
  }

  tokens.push_back(Token{TokenKind::EndOfLine, ""});
  return tokens;
}
