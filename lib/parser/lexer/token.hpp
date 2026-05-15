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

class LexicalError {
  LexicalErrorText message;
  std::size_t column;
};
