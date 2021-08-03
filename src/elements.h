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
  TetrahedronElement() : Element<3>(4, 1){}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
    static const std::vector<std::vector<Precision>> kIntegrationPoints{{0.25, 0.25, 0.25}};  // TODO(dmitrygladky): add correct IP
    return kIntegrationPoints;
  };

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
  TriangleElement() : Element<2>(3, 1){}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
    static const std::vector<std::vector<Precision>> kIntegrationPoints{{1. / 3., 1. / 3.}};
    return kIntegrationPoints;
  };

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
    const Precision xi = ip[0];
    const Precision eta = ip[1];

    // define shape functions
    return {1 - xi - eta, xi, eta};
  }

  MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> &/*ip*/) override {
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

class TriangleElement2 : public Element<2> {
 public:
  TriangleElement2() : Element<2>(6, 1){}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
    static const std::vector<std::vector<Precision>> kIntegrationPoints{
        {1. / 6., 1. / 6.},
        {2. / 3., 1. / 6.},
        {1. / 6., 2. / 3.},
    };
    return kIntegrationPoints;
  };

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

  MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> &/*ip*/) override {
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
  RectangleElement() : Element<2>(4, 1){}

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override {
    static const Precision kIpOffset = std::sqrt(3) / 3;
    static const std::vector<std::vector<Precision>> kIntegrationPoints{
        {-kIpOffset, -kIpOffset},
        {kIpOffset, -kIpOffset},
        {-kIpOffset, kIpOffset},
        {kIpOffset, kIpOffset},
    };
    return kIntegrationPoints;
  };

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
    const Precision eta = ip[0];
    const Precision xi = ip[1];

    // define shape functions
    return {
        (1 - eta) * (1 - xi) / 4,
        (1 + eta) * (1 - xi) / 4,
        (1 + eta) * (1 + xi) / 4,
        (1 - eta) * (1 + xi) / 4,
    };
  }

  MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> &ip) override {
    MatrixFixedRows<2, Precision> dshape = Eigen::Matrix<Precision, 2, 4>();

    const Precision eta = ip[0];
    const Precision xi = ip[1];

    // 1st row
    // dN(i) / dXi
    dshape(0, 0) = (eta - 1.) / 4;
    dshape(0, 1) = (eta - 1.) / 4;
    dshape(0, 2) = (eta + 1.) / 4;
    dshape(0, 3) = (-eta - 1.) / 4;

    // 2nd row
    // dN(i) / dEta
    dshape(1, 0) = (xi - 1.) / 4;
    dshape(1, 1) = (-xi - 1.) / 4;
    dshape(1, 2) = (xi + 1.) / 4;
    dshape(1, 3) = (xi - 1.) / 4;

    return dshape;
  }
};

}  // namespace vulkan_fem
