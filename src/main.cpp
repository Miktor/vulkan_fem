#include <memory>

#include "fem/model.h"
#include "fem/model_factory.h"
#include "fem/solver.h"

int main() {
  auto solver = std::make_shared<vulkan_fem::Solver<2>>();
  auto model = vulkan_fem::ModelFactory::CreateRectangle();
  solver->Solve(*model);
}
