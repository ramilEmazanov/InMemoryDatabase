#include "runner.hpp"

#include <algorithm>
#include <cctype>
#include <istream>
#include <ostream>
#include <string>

#include "command_parser.hpp"
#include "errors.hpp"
#include "reply_formatter.hpp"
#include "tokenizer.hpp"

int Runner::Run() {
  std::string line;

  while (std::getline(in_, line)) {
    auto tokens = Tokenizer::Tokenize(line);

    if (tokens.has_value() && tokens->size() == 1 &&
        tokens->front().kind == TokenKind::kBareword) {
      std::string cmd = tokens->front().lexeme;
      std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
      if (cmd == "EXIT") {
        break;
      }
    }

    if (!tokens.has_value()) {
      error_out_ << "Tokenizing error : " << FormatLexicalError(tokens.error())
                 << '\n';
      continue;
    }

    auto command_parsing_result = CommandParser::Parse(tokens.value());

    if (!command_parsing_result.has_value()) {
      error_out_ << "Parsing error : "
                 << FormatParsingError(command_parsing_result.error()) << '\n';
      continue;
    }

    auto reply = executor_.Execute(command_parsing_result.value());

    ReplyFormatter::Format(reply, out_);
  }

  return 0;
}

Runner::Runner(std::istream& in, std::ostream& out, std::ostream& error_out,
               CommandExecutor& executor)
    : in_(in), out_(out), error_out_(error_out), executor_(executor) {}
