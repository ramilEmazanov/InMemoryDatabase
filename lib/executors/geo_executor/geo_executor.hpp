#pragma once

#include "geo_commands.hpp"
#include "reply.hpp"

class CommandExecutor;

class GeoExecutor {
 public:
  GeoExecutor(CommandExecutor& executor);

  Reply operator()(const GeoAdd& cmd);
  Reply operator()(const GeoPos& cmd);
  Reply operator()(const GeoDist& cmd);
  Reply operator()(const GeoSearch& cmd);
  Reply operator()(const GeoSearchStore& cmd);

 private:
  CommandExecutor& executor_;
};
