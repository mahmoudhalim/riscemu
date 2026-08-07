#include "riscv/logging.h"
#include "riscv/simulator.h"

#include <filesystem>
#include <iostream>
#include <print>
#include <string_view>

namespace {
constexpr std::string_view USAGE =
    "Usage: riscemu [--log-level=<level>] [-v] <executable>\n"
    "  --log-level=<error|warn|info|debug|trace>  log threshold (default "
    "warn)\n"
    "  -v                     shorthand for --log-level=info\n"
    "  -h, --help             show this help\n"
    "Exit status is the guest's exit code; 1 on load failure; 2 on a usage "
    "error.\n";
}

int main(int argc, char** argv) {
  std::filesystem::path program;
  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (arg == "-v") {
      logging::set_level(logging::Level::Info);
    } else if (arg == "-h" || arg == "--help") {
      std::print(std::cerr, "{}", USAGE);
      return 0;
    } else if (arg.starts_with("--log-level=")) {
      const auto parsed = logging::parse_level(arg.substr(12));
      if (!parsed) {
        std::print(std::cerr, "riscemu: invalid log level '{}'\n{}\n",
                   arg.substr(12), USAGE);
        return 2;
      }
      logging::set_level(*parsed);
    } else if (arg.starts_with('-') && arg != "-") {
      std::print(std::cerr, "riscemu: unknown option '{}'\n{}\n", arg, USAGE);
      return 2;
    } else if (!program.empty()) {
      std::print(std::cerr, "riscemu: multiple executables given\n{}\n", USAGE);
      return 2;
    } else {
      program = arg;
    }
  }

  if (program.empty()) {
    std::print(std::cerr, "{}", USAGE);
    return 2;
  }

  Simulator sim;
  auto load = sim.load(program);
  if (!load) {
    std::print(std::cerr, "riscemu: {}\n", load.error());
    return 1;
  }
  auto outcome = sim.run();
  if (!outcome) {
    std::print(std::cerr, "riscemu: guest faulted: {} at pc=0x{:08x}\n",
               to_string(outcome.error().kind), outcome.error().pc);
    return 1;
  }
  return *outcome;
}