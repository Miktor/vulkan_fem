#pragma once

#include "fem.h"
#include <Eigen/Dense>

namespace vulkan_fem {
class Material {
 public:
};

template <uint32_t DIM>
class LinearMaterial {};

template <>
class LinearMaterial<3> : public Material {
 public:
  Eigen::Matrix<Precision, 6, 6> stiffnes_matrix_;

  LinearMaterial(double e, double nu) {
    const auto lambda = static_cast<Precision>(e * nu / (1. + nu) / (1. + 2. * nu));
    const auto mu = static_cast<Precision>(e / 2 / (1. + nu));

    const auto c1 = static_cast<Precision>(lambda + 2. * mu);

    stiffnes_matrix_ << c1, lambda, lambda, .0, .0, .0, lambda, c1, lambda, .0, .0, .0, lambda, lambda, c1, .0, .0, .0, .0, .0, .0, mu, .0,
        .0, .0, .0, .0, .0, mu, .0, .0, .0, .0, .0, .0, mu;
  }

  virtual Eigen::Matrix<Precision, 6, 6> GetStiffnessMatrix() { return stiffnes_matrix_; }

 private:
};

template <>
class LinearMaterial<2> : public Material {
 public:
  Eigen::Matrix<Precision, 3, 3> stiffnes_matrix_;

  LinearMaterial(double e, double nu) {
    stiffnes_matrix_ << 1.0, static_cast<Precision>(nu), .0, static_cast<Precision>(nu), 1.0, .0, 0.0, 0.0,
        static_cast<Precision>((1.0 - nu) / 2.);

    stiffnes_matrix_ *= static_cast<Precision>(e / (1.0 - pow(nu, 2.)));
  }

  virtual Eigen::Matrix<Precision, 3, 3> GetStiffnessMatrix() { return stiffnes_matrix_; }

 private:
};
}  // namespace vulkan_fem
