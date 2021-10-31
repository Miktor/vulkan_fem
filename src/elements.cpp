#include "elements.h"

namespace vulkan_fem {

template<uint32_t DIM>
Eigen::Matrix<Precision, 3, Eigen::Dynamic> Element<DIM>::MakeStrainMatrix(const uint16_t element_count,
                                                                      const MatrixFixedRows<DIM> &elem_matrix) const {
  Eigen::Matrix<Precision, 3, Eigen::Dynamic> strain_matrix(3, element_count * DIM);

  for (int i = 0; i < element_count; ++i) {
    strain_matrix(0, 2 * i + 0) = elem_matrix(0, i);  // Nix
    strain_matrix(0, 2 * i + 1) = 0.0F;

    strain_matrix(1, 2 * i + 0) = 0.0F;
    strain_matrix(1, 2 * i + 1) = elem_matrix(1, i);  // Niy

    strain_matrix(2, 2 * i + 0) = elem_matrix(1, i);  // Niy
    strain_matrix(2, 2 * i + 1) = elem_matrix(0, i);  // Nix
  }

  return strain_matrix;
}

TetrahedronElement::TetrahedronElement() : Element<3>(4, 1) {}

std::vector<std::vector<Precision>> TetrahedronElement::GetIntegrationPoints() const {
  static const std::vector<std::vector<Precision>> kIntegrationPoints{{0.25, 0.25, 0.25}};  // TODO(dmitrygladky): add correct IP
  return kIntegrationPoints;
};

Precision TetrahedronElement::GetIntegrationWeight(uint8_t /*p*/) const { return 1.; }

std::vector<Precision> TetrahedronElement::CalcShape(const std::vector<Precision> &ip) const {
  const Precision xi = ip[0];
  const Precision eta = ip[1];
  const Precision zeta = ip[2];

  // define shape functions
  return {1 - xi - eta - zeta, xi, eta, zeta};
}

MatrixFixedRows<3, Precision> TetrahedronElement::CalcDShape(const std::vector<Precision> & /*ip*/) {
  MatrixFixedRows<3, Precision> dshape = Eigen::Matrix<Precision, 3, 4>();
  // 1st row
  // dN(i) / dXi
  dshape(0, 0) = -1;
  dshape(0, 1) = 1;
  dshape(0, 2) = 0;
  dshape(0, 3) = 0;

  // 2nd row
  // dN(i) / dEta
  dshape(1, 0) = -1;
  dshape(1, 1) = 0;
  dshape(1, 2) = 1;
  dshape(1, 3) = 0;

  // 3rd row
  // dN(i) / dZeta
  dshape(2, 0) = -1;
  dshape(2, 1) = 0;
  dshape(2, 2) = 0;
  dshape(2, 3) = 1;

  return dshape;
}

TriangleElement::TriangleElement() : Element<2>(3, 1) {}

std::vector<std::vector<Precision>> TriangleElement::GetIntegrationPoints() const {
  static const std::vector<std::vector<Precision>> kIntegrationPoints{{1. / 3., 1. / 3.}};
  return kIntegrationPoints;
}

Precision TriangleElement::GetIntegrationWeight(uint8_t /*p*/) const { return 0.5; }

std::vector<Precision> TriangleElement::CalcShape(const std::vector<Precision> &ip) const {
  const Precision xi = ip[0];
  const Precision eta = ip[1];

  // define shape functions
  return {1 - xi - eta, xi, eta};
}

MatrixFixedRows<2, Precision> TriangleElement::CalcDShape(const std::vector<Precision> & /*ip*/) {
  MatrixFixedRows<2, Precision> dshape = Eigen::Matrix<Precision, 2, 3>();
  // 1st row
  // dN(i) / dXi
  dshape(0, 0) = -1.;
  dshape(0, 1) = 1.;
  dshape(0, 2) = .0;

  // 2nd row
  // dN(i) / dEta
  dshape(1, 0) = -1.;
  dshape(1, 1) = .0;
  dshape(1, 2) = 1.;

  return dshape;
}

Triangle2Element::Triangle2Element() : Element<2>(6, 1) {}

std::vector<std::vector<Precision>> Triangle2Element::GetIntegrationPoints() const {
  static const std::vector<std::vector<Precision>> kIntegrationPoints{
      {1. / 6., 1. / 6.},
      {2. / 3., 1. / 6.},
      {1. / 6., 2. / 3.},
  };
  return kIntegrationPoints;
};

Precision Triangle2Element::GetIntegrationWeight(uint8_t /*p*/) const { return 1.; }

std::vector<Precision> Triangle2Element::CalcShape(const std::vector<Precision> &ip) const {
  const Precision xi = ip[0];
  const Precision eta = ip[1];

  // define shape functions
  return {
      xi * (2 * xi - 1),                        //
      eta * (2 * eta - 1),                      //
      (1 - xi - eta) * (1 - 2 * xi - 2 * eta),  //
      4 * xi * eta,                             //
      4 * eta * (1 - xi - eta),                 //
      4 * xi * (1 - xi - eta),                  //
  };
}

MatrixFixedRows<2, Precision> Triangle2Element::CalcDShape(const std::vector<Precision> & /*ip*/) {
  MatrixFixedRows<2, Precision> dshape = Eigen::Matrix<Precision, 2, 3>();
  // 1st row
  // dN(i) / dXi
  dshape(0, 0) = -1.;
  dshape(0, 1) = 1.;
  dshape(0, 2) = .0;

  // 2nd row
  // dN(i) / dEta
  dshape(1, 0) = -1.;
  dshape(1, 1) = .0;
  dshape(1, 2) = 1.;

  return dshape;
}

RectangleElement::RectangleElement() : Element<2>(4, 1) {}

std::vector<std::vector<Precision>> RectangleElement::GetIntegrationPoints() const {
  static const Precision kIpOffset = 1. / std::sqrt(3);
  static const std::vector<std::vector<Precision>> kIntegrationPoints{
      {-kIpOffset, -kIpOffset},
      {kIpOffset, -kIpOffset},
      {-kIpOffset, kIpOffset},
      {kIpOffset, kIpOffset},
  };
  return kIntegrationPoints;
}

Precision RectangleElement::GetIntegrationWeight(uint8_t /*p*/) const { return 1.; }

std::vector<Precision> RectangleElement::CalcShape(const std::vector<Precision> &ip) const {
  const Precision xi = ip[0];   // ξ
  const Precision eta = ip[1];  // η

  // define shape functions
  return {
      (1 - xi) * (1 - eta) / 4,
      (1 + xi) * (1 - eta) / 4,
      (1 + xi) * (1 + eta) / 4,
      (1 - xi) * (1 + eta) / 4,
  };
}

MatrixFixedRows<2, Precision> RectangleElement::CalcDShape(const std::vector<Precision> &ip) {
  MatrixFixedRows<2, Precision> dshape = Eigen::Matrix<Precision, 2, 4>();

  const Precision xi = ip[0];   // ξ
  const Precision eta = ip[1];  // η

  // 1st row
  // dN(i) / dXi
  dshape(0, 0) = (eta - 1.) / 4;
  dshape(0, 1) = (1. - eta) / 4;
  dshape(0, 2) = (eta + 1.) / 4;
  dshape(0, 3) = (-eta - 1.) / 4;

  // 2nd row
  // dN(i) / dEta
  dshape(1, 0) = (xi - 1.) / 4;
  dshape(1, 1) = (-xi - 1.) / 4;
  dshape(1, 2) = (xi + 1.) / 4;
  dshape(1, 3) = (1 - xi) / 4;

  return dshape;
}

Rectangle2Element::Rectangle2Element() : Element<2>(8, 1) {}

std::vector<std::vector<Precision>> Rectangle2Element::GetIntegrationPoints() const {
  static const Precision kIpOffset = std::sqrt(3. / 5.);
  static const std::vector<std::vector<Precision>> kIntegrationPoints{
      {-kIpOffset, -kIpOffset},  //
      {-kIpOffset, 0.},          //
      {-kIpOffset, kIpOffset},   //
      {0., -kIpOffset},          //
      {0., 0.},                  //
      {0., kIpOffset},           //
      {kIpOffset, -kIpOffset},   //
      {kIpOffset, 0.},           //
      {kIpOffset, kIpOffset},    //
  };
  return kIntegrationPoints;
}

Precision Rectangle2Element::GetIntegrationWeight(uint8_t p) const {
  constexpr Precision kA = 5. / 9.;
  constexpr Precision kB = 8. / 9.;
  constexpr Precision kASqr = kA * kA;
  constexpr Precision kBSqr = kB * kB;
  constexpr Precision kAB = kA * kB;

  constexpr Precision kW[9] = {kASqr, kAB, kASqr, kAB, kBSqr, kAB, kASqr, kAB, kASqr};

  return kW[p];
}

std::vector<Precision> Rectangle2Element::CalcShape(const std::vector<Precision> &ip) const {
  const Precision xi = ip[0];   // ξ
  const Precision eta = ip[1];  // η

  // define shape functions
  return {
      (1 - xi) * (1 - eta) * (-xi - eta - 1) / 2,  //
      (1 + xi) * (1 - eta) * (xi - eta - 1) / 2,   //
      (1 + xi) * (1 + eta) * (xi + eta - 1) / 2,   //
      (1 - xi) * (1 + eta) * (-xi + eta - 1) / 2,  //
      (1 - eta) * (1 + xi) * (1 - xi) / 2,         //
      (1 + xi) * (1 + eta) * (1 - eta) / 2,        //
      (1 + eta) * (1 + xi) * (1 - xi) / 2,         //
      (1 - xi) * (1 + eta) * (1 - eta) / 2,        //
  };
}

MatrixFixedRows<2, Precision> Rectangle2Element::CalcDShape(const std::vector<Precision> &ip) {
  MatrixFixedRows<2, Precision> dshape = Eigen::Matrix<Precision, 2, 8>();

  const Precision xi = ip[0];   // ξ
  const Precision eta = ip[1];  // η

  // 1st row
  // dN(i) / dXi

  dshape(0, 0) = -(-1 + eta) * (eta + 2 * xi) / 4;
  dshape(0, 1) = (-1 + eta) * (eta - 2 * xi) / 4;
  dshape(0, 2) = (1 + eta) * (eta + 2 * xi) / 4;
  dshape(0, 3) = -(1 + eta) * (eta - 2 * xi) / 4;
  dshape(0, 4) = (-1 + eta) * xi;
  dshape(0, 5) = -(-1 + eta) * (1 + eta) / 2;
  dshape(0, 6) = -(1 + eta) * xi;
  dshape(0, 7) = (-1 + eta) * (1 + eta) / 2;

  // 2nd row
  // dN(i) / dEta
  dshape(1, 0) = -(-1 + xi) * (2 * eta + xi) / 4;
  dshape(1, 1) = (2 * eta - xi) * (1 + xi) / 4;
  dshape(1, 2) = (1 + xi) * (2 * eta + xi) / 4;
  dshape(1, 3) = -(2 * eta - xi) * (-1 + xi) / 4;
  dshape(1, 4) = (-1 + xi) * (1 + xi) / 2;
  dshape(1, 5) = -eta * (1 + xi);
  dshape(1, 6) = -(-1 + xi) * (1 + xi) / 2;
  dshape(1, 7) = eta * (-1 + xi);

  return dshape;
}

}  // namespace vulkan_fem
