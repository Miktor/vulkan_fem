#pragma once

#include "fem.h"
#include <Eigen/Dense>
#include <vector>

namespace vulkan_fem {

// space dimention
template <uint32_t DIM = 3>
class Element {
 public:
  // nuber of dofs
  [[nodiscard]] uint32_t GetElementCount() const { return element_count_; }
  // element order
  [[nodiscard]] uint32_t GetOrder() const { return order_; }

  [[nodiscard]] virtual std::vector<std::vector<Precision>> GetIntegrationPoints() const = 0;
  [[nodiscard]] virtual Precision GetIntegrationWeight(uint8_t p) const = 0;

  // calculate shape functions
  // ip - integration point
  // shape - shape function values
  [[nodiscard]] virtual std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const = 0;

  // calculate partial derivatives
  virtual MatrixFixedRows<DIM, Precision> CalcDShape(const std::vector<Precision> &ip) = 0;

 protected:
  Element(uint32_t element_count, uint32_t order) : element_count_(element_count), order_(order) {}
  virtual ~Element() = default;

 private:
  uint32_t element_count_;
  uint32_t order_;
};

class ElementIndices {};

template <uint32_t DIM = 3>
class ElementTransformations {};

class TetrahedronElement : public Element<3> {
 public:
  TetrahedronElement() : Element<3>(4, 1) {}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
    static const std::vector<std::vector<Precision>> kIntegrationPoints{{0.25, 0.25, 0.25}};  // TODO(dmitrygladky): add correct IP
    return kIntegrationPoints;
  };

  [[nodiscard]] virtual Precision GetIntegrationWeight(uint8_t /*p*/) const override { return 1.; }

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
    const Precision xi = ip[0];
    const Precision eta = ip[1];
    const Precision zeta = ip[2];

    // define shape functions
    return {1 - xi - eta - zeta, xi, eta, zeta};
  }

  MatrixFixedRows<3, Precision> CalcDShape(const std::vector<Precision> & /*ip*/) override {
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
};

class TriangleElement : public Element<2> {
 public:
  TriangleElement() : Element<2>(3, 1) {}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
    static const std::vector<std::vector<Precision>> kIntegrationPoints{{1. / 3., 1. / 3.}};
    return kIntegrationPoints;
  };

  [[nodiscard]] virtual Precision GetIntegrationWeight(uint8_t /*p*/) const override { return 0.5; }

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
    const Precision xi = ip[0];
    const Precision eta = ip[1];

    // define shape functions
    return {1 - xi - eta, xi, eta};
  }

  MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> & /*ip*/) override {
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
};

class Triangle2Element : public Element<2> {
 public:
  Triangle2Element() : Element<2>(6, 1) {}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
    static const std::vector<std::vector<Precision>> kIntegrationPoints{
        {1. / 6., 1. / 6.},
        {2. / 3., 1. / 6.},
        {1. / 6., 2. / 3.},
    };
    return kIntegrationPoints;
  };

  [[nodiscard]] virtual Precision GetIntegrationWeight(uint8_t /*p*/) const override { return 1.; }

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
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

  MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> & /*ip*/) override {
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
};

class RectangleElement : public Element<2> {
 public:
  RectangleElement() : Element<2>(4, 1) {}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
    static const Precision kIpOffset = 1. / 3. / std::sqrt(3);
    static const std::vector<std::vector<Precision>> kIntegrationPoints{
        {-kIpOffset, -kIpOffset},
        {kIpOffset, -kIpOffset},
        {-kIpOffset, kIpOffset},
        {kIpOffset, kIpOffset},
    };
    return kIntegrationPoints;
  };

  [[nodiscard]] virtual Precision GetIntegrationWeight(uint8_t /*p*/) const override { return 1.; }

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
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

  MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> &ip) override {
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
};

class Rectangle2Element : public Element<2> {
 public:
  Rectangle2Element() : Element<2>(8, 1) {}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
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
  };

  [[nodiscard]] virtual Precision GetIntegrationWeight(uint8_t p) const override {
    constexpr Precision kA = 5. / 9.;
    constexpr Precision kB = 8. / 9.;
    constexpr Precision kASqr = kA * kA;
    constexpr Precision kBSqr = kB * kB;
    constexpr Precision kAB = kA * kB;

    constexpr Precision kW[9] = {kASqr, kAB, kASqr, kAB, kBSqr, kAB, kASqr, kAB, kASqr};

    return kW[p];
  }

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
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

  MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> &ip) override {
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
};

}  // namespace vulkan_fem
