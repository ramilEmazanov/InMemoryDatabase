#include "runner.hpp"

#include "command_parser.hpp"
#include "token.hpp"
#include "tokenizer.hpp"

int Runner::Run() {
  std::string line;

  while (std::getline(in_, line)) {
    auto tokens = Tokenizer::Tokenize(line);

    if (!tokens.has_value()) {
      error_out_ << tokens.error().message << '\n';
      continue;
    }

    auto command = CommandParser::Parse(tokens.value());

    auto reply = executor_.Execute(command.value());

    
  }
}

Runner::Runner(std::istream& in, std::ostream& out, std::ostream& error_out,
               CommandExecutor& executor)
    : in_(in), out_(out), error_out_(error_out), executor_(executor) {}
