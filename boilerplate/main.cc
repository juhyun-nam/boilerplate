#include <sstream>
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include "gsl/span"
#include "gsl/string_span"

int main(int argc, char** argv) {
  gsl::span<gsl::zstring<>> args(argv, argc);

  spdlog::info("HELLO WORLD");
  std::stringstream str("{ \"opt1\": \"value1\" }");
  nlohmann::json config;
  str >> config;

  return 0;
}