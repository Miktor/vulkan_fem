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

template <uint32_t DIM = 3>
class ScalarElement : public Element<DIM> {
 public:
  // calculate shape functions
  // ip - integration point
  // shape - shape function values
  [[nodiscard]] virtual std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const = 0;

  // calculate partial derivatives
  virtual MatrixFixedRows<DIM, Precision> CalcDShape(const std::vector<Precision> &ip) = 0;

 protected:
  ScalarElement(uint32_t element_count, uint32_t order) : Element<DIM>(element_count, order) {}
};

class TetrahedronElement : public ScalarElement<3> {
 public:
  TetrahedronElement() : ScalarElement<3>(4, 1) {}

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
    const Precision xi = ip[0];
    const Precision eta = ip[1];
    const Precision zeta = ip[2];

    // define shape functions
    return {1 - xi - eta - zeta, xi, eta, zeta};
  }

  MatrixFixedRows<3, Precision> CalcDShape(const std::vector<Precision> &) override {
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

class TriangleElement : public ScalarElement<2> {
 public:
  TriangleElement() : ScalarElement<2>(3, 1) {}

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override {
    const Precision xi = ip[0];
    const Precision eta = ip[1];

    // define shape functions
    return {1 - xi - eta, xi, eta};
  }

  MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> &) override {
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

class Element2dTo3d : public ScalarElement<3> {
 private:
  std::shared_ptr<ScalarElement<2>> elem_;

 public:
  explicit Element2dTo3d(const std::shared_ptr<ScalarElement<2>> &elem)
      : ScalarElement<3>(elem->GetElementCount(), elem->GetOrder()), elem_(elem) {}

  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override { return elem_->CalcShape(ip); }

  MatrixFixedRows<3, Precision> CalcDShape(const std::vector<Precision> &ip) override {
    auto dshape = elem_->CalcDShape(ip);
    MatrixFixedRows<3, Precision> converted = MatrixFixedRows<3, Precision>::Zero(3, dshape.cols());
    converted.topRows(2) = dshape.topRows(2);
    return converted;
  }
};

}  // namespace vulkan_fem
