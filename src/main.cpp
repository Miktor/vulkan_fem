
#include "fem_application.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <exception>
#include <iostream>

int main(const int /*argc*/, const char ** /*argv[]*/) {
  spdlog::info("Start");

  FEMApplication app;

  try {
    app.Run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
