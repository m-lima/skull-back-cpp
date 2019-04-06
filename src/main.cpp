#include "server.hpp"

#include <mfl/args.hpp>

int main(int argc, char * argv[]) {
  auto aHost = mfl::args::extractOption(argc, argv, "-h");
  auto aPort = mfl::args::extractOption(argc, argv, "-p");
  auto aThreadCount = mfl::args::extractOption(argc, argv, "-t");

  std::string host{aHost ? aHost : "localhost"};
  std::uint16_t port{static_cast<uint16_t>(aPort ? std::atoi(aPort) : 8080)};
  std::uint16_t threadCount{static_cast<uint16_t>(aThreadCount ? std::atoi(aThreadCount) : 4)};

  server::listen(std::move(host), port, threadCount);
  return 0;
}
