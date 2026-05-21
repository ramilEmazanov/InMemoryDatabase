#include "command_executor.hpp"

#include "general_executor.hpp"
#include "geo_executor.hpp"
#include "list_executor.hpp"
#include "overload.hpp"
#include "set_executor.hpp"
#include "string_executor.hpp"

Reply CommandExecutor::Execute(const ParsedCommand& command) {
  return std::visit(
      Overload{
          StringExecutor{*this},
          ListExecutor{*this},
          SetExecutor{*this},
          GeoExecutor{*this},
          GeneralExecutor{*this},
      },
      command);
}
