#include <types.hpp>
#include <storage.hpp>
#include <parser.hpp>
#include <commands.hpp>

int main(int argc, const char** argv) {
  Storage storage;
  ParseCli(argc, argv, storage);

}