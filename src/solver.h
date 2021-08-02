#pragma once

#include "fem.h"
#include "model.h"
#include <iostream>

namespace vulkan_fem {

template <uint32_t DIM = 3>
class Solver {
 public:
  void Solve(Model<DIM> &model) {
    auto global_stiffnes_matrix = model.BuildGlobalStiffnesMatrix();  // K_global
    std::cout << "global_stiffnes_matrix: " << global_stiffnes_matrix << std::endl;

    model.ApplyConstraints(global_stiffnes_matrix);

    Eigen::SimplicialLDLT<decltype(global_stiffnes_matrix)> solver(global_stiffnes_matrix);
    std::cout << "global_stiffnes_matrix: " << global_stiffnes_matrix << std::endl;
    Eigen::VectorXf displacements = solver.solve(model.GetLoads());

    std::cout << "displacements: " << displacements << std::endl;

    model.AccountDisplacements(displacements);

    std::cout << "new coords: " << model.GetVertices() << std::endl;
  }
};

}  // namespace vulkan_fem
