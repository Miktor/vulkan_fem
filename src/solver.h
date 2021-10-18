#pragma once

#include "fem.h"
#include "model.h"
#include <iostream>

namespace vulkan_fem {

template <uint32_t DIM = 3>
class Solver {
 public:
  void Solve(Model<DIM> &model) {
    auto global_stiffness_matrix = model.BuildGlobalStiffnessMatrix();  // K_global
    std::cout << "global_stiffness_matrix: " << global_stiffness_matrix << std::endl;

    model.ApplyConstraints(global_stiffness_matrix);

    Eigen::SimplicialLDLT<decltype(global_stiffness_matrix)> solver(global_stiffness_matrix);
    std::cout << "global_stiffness_matrix: " << global_stiffness_matrix << std::endl;
    Eigen::VectorXf displacements = solver.solve(model.GetLoads());

    std::cout << "displacements: " << displacements << std::endl;

    model.AccountDisplacements(displacements);

    std::cout << "new coords: " << model.GetVertices() << std::endl;
  }
};

}  // namespace vulkan_fem
