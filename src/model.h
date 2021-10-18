#pragma once

#include "elements.h"
#include "enumerate.h"
#include "fem.h"
#include "material.h"
#include "spdlog/fmt/ostr.h"
#include <spdlog/spdlog.h>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace vulkan_fem {

using Vertex3 = Eigen::Matrix<Precision, 3, 1>;

struct Constraint {
  enum Type { kUx = 1 << 0, kUy = 1 << 1, kUz = 1 << 2, kUxy = kUx | kUy, kUxz = kUx | kUz, kUyz = kUy | kUz, kUxyz = kUx | kUy | kUz };

  uint32_t node_;
  Type type_;
};

template <uint32_t DIM>
struct Load {
  uint32_t node_;
  Precision forces_[DIM];
};

template <uint32_t DIM>
struct DimentionHelper;

template <>
struct DimentionHelper<2> {};

template <uint32_t DIM = 3>
class Model {
 public:
  using ElementMatrix = Eigen::SparseMatrix<Precision>;
  using Loads = Eigen::Matrix<Precision, 1, Eigen::Dynamic>;

  Model(std::shared_ptr<Element<DIM>> element_type, std::vector<Vertex3> vertices, std::vector<uint16_t> indices,
        std::vector<Constraint> constraints, const std::vector<Load<DIM>> &loads, double e, double mu)
      : element_type_(std::move(element_type)),
        material_(e, mu),
        elements_(std::move(vertices)),
        element_indices_(std::move(indices)),
        constraints_(std::move(constraints)),
        loads_(BuildLoadsVector(loads)) {}

  [[nodiscard]] const std::vector<Vertex3> &GetVertices() const { return elements_; }

  [[nodiscard]] const std::vector<uint16_t> &GetIndices() const { return element_indices_; }

  [[nodiscard]] Eigen::VectorXf GetLoads() const { return loads_; }

  [[nodiscard]] std::shared_ptr<Element<DIM>> GetElementType() const { return element_type_; }

  void AccountDisplacements(const Eigen::VectorXf &displacements) {
    if (displacements.size() / DIM != elements_.size()) {
      throw std::runtime_error("displacements.size() / DIM != elements_.size()");
    }

    int current = 0;
    if (displacements.rows() != elements_.size() * DIM) {
      throw std::runtime_error("invalid displacement count");
    }

    for (int c = 0; c < displacements.rows(); c += DIM) {
      for (int i = 0; i < DIM; ++i) {
        switch (i) {
          case 0:
          case 1:
          case 2:
            elements_[current][i] += displacements[c + i];
            break;
          default:
            throw std::runtime_error("Invalid dims");
        }
      }
      ++current;
    }
  }

  ElementMatrix BuildGlobalStiffnessMatrix() {
    const uint32_t element_count = element_type_->GetElementCount();
    const uint32_t number_of_elements = element_indices_.size() / element_count;

    // const uint32_t order = element_type_->GetOrder();

    using T = Eigen::Triplet<Precision>;
    std::vector<T> triplets;
    triplets.reserve(element_count * element_count * number_of_elements);

    const auto d_matrix = material_.GetStiffnessMatrix();
    spdlog::info("\\nD: {}", d_matrix);

    MatrixFixedCols<DIM> elem_transform(element_count, DIM);
    elem_transform.setZero();
    for (uint32_t index = 0; index < element_indices_.size();) {
      // put all vertex transforms into matrix
      for (uint16_t sub_index = 0; sub_index < element_count && index + sub_index < element_indices_.size(); ++sub_index) {
        const uint16_t sub_element_index = element_indices_[index + sub_index];
        const Vertex3 &sub_element_vertex = elements_[sub_element_index];

        for (int i = 0; i < DIM; ++i) {
          elem_transform.row(sub_index)[i] = sub_element_vertex[i];
        }
      }

      spdlog::info("\nelem_transform: {}", elem_transform);
      Eigen::Matrix<Precision, Eigen::Dynamic, Eigen::Dynamic> element_stiffness_matrix;
      element_stiffness_matrix.setZero(element_count * DIM, element_count * DIM);
      for (const auto &[elem_matrix, w, J_det] : CalcElementMatrix(element_type_, elem_transform)) {
        const auto b_matrix = MakeStrainMatrix(element_count, elem_matrix);
        spdlog::info("\nB: {}", b_matrix);

        element_stiffness_matrix += b_matrix.transpose() * d_matrix * b_matrix * J_det * w;
      }

      spdlog::info("\\nK: {}", element_stiffness_matrix);

      for (uint32_t i = 0; i < element_count; ++i) {
        for (uint32_t j = 0; j < element_count; ++j) {
          const uint16_t global_index_i = element_indices_[index + i];
          const uint16_t global_index_j = element_indices_[index + j];

          // x coords
          triplets.emplace_back(DIM * global_index_i + 0, DIM * global_index_j + 0, element_stiffness_matrix(DIM * i + 0, DIM * j + 0));
          triplets.emplace_back(DIM * global_index_i + 0, DIM * global_index_j + 1, element_stiffness_matrix(DIM * i + 0, DIM * j + 1));

          // y coords
          triplets.emplace_back(DIM * global_index_i + 1, 2 * global_index_j + 0, element_stiffness_matrix(DIM * i + 1, DIM * j + 0));
          triplets.emplace_back(DIM * global_index_i + 1, 2 * global_index_j + 1, element_stiffness_matrix(DIM * i + 1, DIM * j + 1));
        }
      }

      index += element_count;
    }

    ElementMatrix global_stiffness_matrix(elements_.size() * DIM, elements_.size() * DIM);
    global_stiffness_matrix.setZero();
    global_stiffness_matrix.setFromTriplets(triplets.begin(), triplets.end());
    return global_stiffness_matrix;
  }

  void ApplyConstraints(ElementMatrix &global_stiffnes_matrix) {
    std::vector<int> indices_to_constraint;

    for (const auto contraint : constraints_) {
      switch (DIM) {
        case 3:
          if ((contraint.type_ & Constraint::kUz) != 0) {
            indices_to_constraint.push_back(DIM * contraint.node_ + 2);
          }
        case 2:
          if ((contraint.type_ & Constraint::kUy) != 0) {
            indices_to_constraint.push_back(DIM * contraint.node_ + 1);
          }
        case 1:
          if ((contraint.type_ & Constraint::kUx) != 0) {
            indices_to_constraint.push_back(DIM * contraint.node_ + 0);
          }
        default:
          break;
      }
    }

    for (int k = 0; k < global_stiffnes_matrix.outerSize(); ++k) {
      for (ElementMatrix::InnerIterator it(global_stiffnes_matrix, k); it; ++it) {
        for (auto index : indices_to_constraint) {
          if (it.row() == index || it.col() == index) {
            it.valueRef() = it.row() == it.col() ? 1.0F : 0.0F;
          }
        }
      }
    }
  }

 private:
  Loads BuildLoadsVector(const std::vector<Load<DIM>> &loads) {
    Loads load_vector = Loads::Zero(elements_.size() * DIM);
    for (auto load : loads) {
      for (uint32_t i = 0; i < DIM; ++i) {
        load_vector[load.node_ * DIM + i] = load.forces_[i];
      }
    }
    return load_vector;
  }

  // N matrix
  // [ Ni 0
  // [ 0  Ni
  // [ Ni Ni
  std::vector<std::tuple<MatrixFixedRows<DIM>, Precision, Precision>> CalcElementMatrix(std::shared_ptr<Element<DIM>> element_type,
                                                                                        const MatrixFixedCols<DIM> &elem_transform) {
    std::vector<std::tuple<MatrixFixedRows<DIM>, Precision, Precision>> result;

    for (const auto &[i, ip] : Enumerate(element_type->GetIntegrationPoints())) {
      const auto dshape = element_type->CalcDShape(ip);

      // build jacobian (d(x, y, z)/d(xi, eta, zeta))
      const MatrixDim<DIM> jacobian = dshape * elem_transform;
      const Precision jacobian_det = jacobian.determinant();
      const MatrixDim<DIM> inverse_jacobian = jacobian.inverse();

      // element matrix
      const auto element_matrix = inverse_jacobian * dshape;

      spdlog::info("\\ndshape: {}", dshape);
      spdlog::info("\\nJac: {}", jacobian);
      spdlog::info("\\nJDet: {}", jacobian_det);
      spdlog::info("\\nInvJ: {}", inverse_jacobian);
      spdlog::info("\\nE: {}", element_matrix);

      result.push_back(std::make_tuple(element_matrix, element_type->GetIntegrationWeight(i), jacobian_det));
    }

    return result;
  }

  // B matrix
  // [ Nix 0
  // [ 0   Niy
  // [ Niy Nix
  Eigen::Matrix<Precision, 3, Eigen::Dynamic> MakeStrainMatrix(const uint16_t element_count, const MatrixFixedRows<DIM> &elem_matrix) {
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

  std::shared_ptr<Element<DIM>> element_type_;

  LinearMaterial<DIM> material_;
  std::vector<Vertex3> elements_;
  std::vector<uint16_t> element_indices_;
  std::vector<ElementTransformations<DIM>> element_transformations_;
  std::vector<Constraint> constraints_;
  Loads loads_;
};

}  // namespace vulkan_fem

namespace std {

template <typename T, typename V>
T &operator<<(T &os, const std::vector<V> &data) {
  for (auto it = data.begin(); it != data.end(); ++it) {
    if (it != data.begin()) {
      os << std::endl;
    }
    os << *it;
  }
  return os;
}
}  // namespace std