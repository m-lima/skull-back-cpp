#include <mfl/args.hpp>

#include "server.hpp"

int main(int argc, char * argv[]) {
  auto aHost = mfl::args::extractOption(argc, argv, "-h");
  auto aPort = mfl::args::extractOption(argc, argv, "-p");
  auto aThreadCount = mfl::args::extractOption(argc, argv, "-t");

  std::string host{aHost ? aHost : "localhost"};
  std::uint16_t port{static_cast<uint16_t>(aPort ? std::strtol(aPort, nullptr, 0) : 8080)};
  std::uint16_t threadCount{static_cast<uint16_t>(aThreadCount ? std::strtol(aThreadCount, nullptr, 0) : 4)};

  server::listen(std::move(host), port, threadCount);
  return 0;
}
