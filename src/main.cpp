#include "server.hpp"

#include <mfl/args.hpp>
#include <mfl/out.hpp>

int main(int argc, char * argv[]) {

  for (int i = 1; i < argc; ++i) {
    mfl::out::println("{} :: '{}'", i, argv[i]);
  }

  auto aHost = mfl::args::extractOption(argc, argv, "-h");
  auto aPort = mfl::args::extractOption(argc, argv, "-p");

  std::string host{aHost ? aHost : "localhost"};
  std::uint16_t port{static_cast<uint16_t>(aPort ? std::atoi(aPort) : 8080)};

  server::listen(std::move(host), port);
  return 0;
}
