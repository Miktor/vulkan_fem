#ifndef Material_h__
#define Material_h__

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
  Eigen::Matrix<precision, 6, 6> stiffnesMatrix;

  LinearMaterial(double E, double nu) {
    const precision lambda = (precision)(E * nu / (1. + nu) / (1. + 2. * nu));
    const precision mu = (precision)(E / 2 / (1. + nu));

    const precision c1 = (precision)(lambda + 2. * mu);

    stiffnesMatrix << c1, lambda, lambda, .0, .0, .0, lambda, c1, lambda, .0, .0, .0, lambda, lambda, c1, .0,
        .0, .0, .0, .0, .0, mu, .0, .0, .0, .0, .0, .0, mu, .0, .0, .0, .0, .0, .0, mu;
  }

  virtual Eigen::Matrix<precision, 6, 6> GetStiffnessMatrix() { return stiffnesMatrix; }

 private:
};

template <>
class LinearMaterial<2> : public Material {
 public:
  Eigen::Matrix<precision, 3, 3> stiffnesMatrix;

  LinearMaterial(double E, double nu) {
    stiffnesMatrix << 1.0, (precision)(nu), .0, (precision)(nu), 1.0, .0, 0.0, 0.0,
        (precision)((1.0 - nu) / 2.);

    stiffnesMatrix *= (precision)(E / (1.0 - pow(nu, 2.)));
  }

  virtual Eigen::Matrix<precision, 3, 3> GetStiffnessMatrix() { return stiffnesMatrix; }

 private:
};
}  // namespace vulkan_fem

#endif  // Material_h__
