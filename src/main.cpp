#include "server.hpp"

#include <mfl/args.hpp>

int main(int argc, char * argv[]) {

  auto aHost = mfl::args::extractOption(argc, argv, "-h");
  auto aPort = mfl::args::extractOption(argc, argv, "-p");
  auto aQuickValuePath = mfl::args::extractOption(argc, argv, "-q");
  auto aSkullPath = mfl::args::extractOption(argc, argv, "-s");

  std::string host{aHost ? aHost : "localhost"};
  std::uint16_t port{static_cast<uint16_t>(aPort ? std::atoi(aPort) : 8080)};
  std::string_view quickValuePath{aQuickValuePath ? aQuickValuePath : "res/quick_values.json"};
  std::string_view skullPath{aSkullPath ? aSkullPath : "res/skull.json"};

  server::listen(std::move(host), port, quickValuePath, skullPath);
  return 0;
}
