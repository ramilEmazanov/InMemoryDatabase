#include "types.hpp"

#include <variant>

#include "overload.hpp"

std::size_t CountValueMemoryUsage(const Value& val) {
  auto overload = Overload(
      [&](const StringValue& str) -> std::size_t {
        return sizeof(StringValue) + str.size();
      },
      [&](const ListValue& str) -> std::size_t {
        std::size_t sum = sizeof(ListValue);

        for (const auto& val : str) {
          sum += val.size() + sizeof(std::string);
        }

        return sum;
      },
      [&](const GeoValue& str) -> std::size_t {},
      [&](const SetValue& str) -> std::size_t {
        std::size_t sum = sizeof(SetValue);

        for (const auto& val : str) {
          sum += val.size() + sizeof(std::string);
        }

        return sum;
      });

  return std::visit(overload, val);
}