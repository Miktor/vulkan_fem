#ifndef elements_h__
#define elements_h__

#include "fem.h"
#include <Eigen/Dense>
#include <vector>

namespace vulkan_fem {

// space dimention
template <uint32_t DIM = 3>
class Element {
 public:
  // nuber of dofs
  uint32_t GetElementCount() const { return element_count_; }
  // element order
  uint32_t GetOrder() const { return order_; }

 protected:
  Element(uint32_t element_count, uint32_t order) : element_count_(element_count), order_(order) {}
  virtual ~Element() {}

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
  virtual std::vector<precision> CalcShape(const std::vector<precision> &ip) const = 0;

  // calculate partial derivatives
  virtual MatrixFixedRows<DIM, precision> CalcDShape(const std::vector<precision> &ip) = 0;

 protected:
  ScalarElement(uint32_t element_count, uint32_t order) : Element<DIM>(element_count, order) {}
};

class TetrahedronElement : public ScalarElement<3> {
 public:
  TetrahedronElement() : ScalarElement<3>(4, 1) {}

  virtual std::vector<precision> CalcShape(const std::vector<precision> &ip) const override {
    const precision xi = ip[0];
    const precision eta = ip[1];
    const precision zeta = ip[2];

    // define shape functions
    return {1 - xi - eta - zeta, xi, eta, zeta};
  }

  virtual MatrixFixedRows<3, precision> CalcDShape(const std::vector<precision> &) override {
    MatrixFixedRows<3, precision> dshape = Eigen::Matrix<precision, 3, 4>();
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

  virtual std::vector<precision> CalcShape(const std::vector<precision> &ip) const override {
    const precision xi = ip[0];
    const precision eta = ip[1];

    // define shape functions
    return {1 - xi - eta, xi, eta};
  }

  virtual MatrixFixedRows<2, precision> CalcDShape(const std::vector<precision> &) override {
    MatrixFixedRows<2, precision> dshape = Eigen::Matrix<precision, 2, 3>();
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
  Element2dTo3d(std::shared_ptr<ScalarElement<2>> elem) : ScalarElement<3>(elem->GetElementCount(), elem->GetOrder()), elem_(elem) {}

  virtual std::vector<precision> CalcShape(const std::vector<precision> &ip) const override { return elem_->CalcShape(ip); }

  virtual MatrixFixedRows<3, precision> CalcDShape(const std::vector<precision> &ip) override {
    auto dshape = elem_->CalcDShape(ip);
    MatrixFixedRows<3, precision> converted = MatrixFixedRows<3, precision>::Zero(3, dshape.cols());
    converted.topRows(2) = dshape.topRows(2);
    return converted;
  }
};

}  // namespace vulkan_fem

#endif  // elements_h__
