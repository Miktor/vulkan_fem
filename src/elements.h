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
  [[nodiscard]] virtual MatrixFixedRows<DIM, Precision> CalcDShape(const std::vector<Precision> &ip) = 0;

  // B matrix
  // [ Nix 0
  // [ 0   Niy
  // [ Niy Nix
  [[nodiscard]] virtual Eigen::Matrix<Precision, 3, Eigen::Dynamic> MakeStrainMatrix(const uint16_t element_count,
                                                                                     const MatrixFixedRows<DIM> &elem_matrix) const;

 protected:
  Element(uint32_t element_count, uint32_t order) : element_count_(element_count), order_(order) {}
  virtual ~Element() = default;

 private:
  const uint32_t element_count_;
  const uint32_t order_;
};

class ElementIndices {};

template <uint32_t DIM = 3>
class ElementTransformations {};

class TetrahedronElement : public Element<3> {
 public:
  TetrahedronElement();

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override;
  [[nodiscard]] Precision GetIntegrationWeight(uint8_t /*p*/) const override;
  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override;
  [[nodiscard]] MatrixFixedRows<3, Precision> CalcDShape(const std::vector<Precision> & /*ip*/) override;
};

class TriangleElement : public Element<2> {
 public:
  TriangleElement();

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override;
  [[nodiscard]] Precision GetIntegrationWeight(uint8_t /*p*/) const override;
  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override;
  [[nodiscard]] MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> & /*ip*/) override;
};

class Triangle2Element : public Element<2> {
 public:
  Triangle2Element();

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override;
  [[nodiscard]] Precision GetIntegrationWeight(uint8_t /*p*/) const override;
  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override;
  [[nodiscard]] MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> & /*ip*/) override;
};

class RectangleElement : public Element<2> {
 public:
  RectangleElement();

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override;
  [[nodiscard]] Precision GetIntegrationWeight(uint8_t /*p*/) const override;
  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override;
  [[nodiscard]] MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> &ip) override;
};

class Rectangle2Element : public Element<2> {
 public:
  Rectangle2Element();

  [[nodiscard]] std::vector<std::vector<Precision>> GetIntegrationPoints() const override;
  [[nodiscard]] Precision GetIntegrationWeight(uint8_t p) const override;
  [[nodiscard]] std::vector<Precision> CalcShape(const std::vector<Precision> &ip) const override;
  [[nodiscard]] MatrixFixedRows<2, Precision> CalcDShape(const std::vector<Precision> &ip) override;
};

}  // namespace vulkan_fem
