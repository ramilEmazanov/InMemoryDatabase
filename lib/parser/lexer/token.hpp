#include <string>

enum class TokenKind { Bareword, Quoted, EndOfLine };

enum class LexicalErrorText {
  UnterminatedString,
  InvalidEscapeSequence,
  UnexpectedControlCharacter
};

struct Token {
  TokenKind kind;
  std::string lexeme;
};

struct LexicalError {
  LexicalErrorText err;
  std::string message;
};
