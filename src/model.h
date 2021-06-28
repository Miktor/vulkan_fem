#ifndef model_h__
#define model_h__

#include "elements.h"
#include "fem.h"
#include "material.h"

namespace vulkan_fem {

struct Vertex3 {
  precision x;
  precision y;
  precision z;
};

struct Constraint {
  enum Type { UX = 1 << 0, UY = 1 << 1, UZ = 1 << 2, UXY = UX | UY, UXZ = UX | UZ, UYZ = UY | UZ, UXYZ = UX | UY | UZ };

  uint32_t node;
  Type type;
};

template <uint32_t DIM>
struct Load {
  uint32_t node;
  precision forces[DIM];
};

template <uint32_t DIM>
struct DimentionHelper;

template <>
struct DimentionHelper<2> {};

template <uint32_t DIM = 3>
class Model {
 public:
  typedef Eigen::SparseMatrix<precision> ElementMatrix;
  typedef Eigen::Matrix<precision, 1, Eigen::Dynamic> Loads;

  Model(std::shared_ptr<ScalarElement<DIM>> element_type, const std::vector<Vertex3> &vertices, const std::vector<uint16_t> &indices,
        const std::vector<Constraint> &constraints, const std::vector<Load<DIM>> &loads, double E, double mu)
      : element_type_(element_type),
        material_(E, mu),
        elements_(vertices),
        element_inidices_(indices),
        constraints_(constraints),
        loads_(buildLoadsVector(loads)) {}

  const std::vector<Vertex3> &GetVertices() const { return elements_; }

  const std::vector<uint16_t> &GetIndices() const { return element_inidices_; }

  Eigen::VectorXf GetLoads() { return loads_; }

  ElementMatrix BuildGlobalStiffnesMatrix() {
    ElementMatrix global_stiffness_matrix(elements_.size() * DIM, elements_.size() * DIM);

    const uint32_t number_of_elements = element_inidices_.size() / DIM;

    const uint32_t element_count = element_type_->GetElementCount();
    // const uint32_t order = element_type_->GetOrder();

    MatrixFixedCols<DIM> elem_transform(element_count, DIM);
    std::vector<precision> jacobian_determinants;

    typedef Eigen::Triplet<precision> T;
    std::vector<T> triplets;
    triplets.reserve(element_count * element_count * number_of_elements);

    for (uint32_t index = 0; index < element_inidices_.size();) {
      // put all vertex transforms into matrix
      for (uint16_t sub_index = 0; sub_index < element_count && index + sub_index < element_inidices_.size(); ++sub_index) {
        const uint16_t sub_element_index = element_inidices_[index + sub_index];
        const Vertex3 &sub_element_vertex = elements_[sub_element_index];

        if (DIM == 2)
          elem_transform.row(sub_index) << sub_element_vertex.x, sub_element_vertex.y;
        else if (DIM == 3)
          elem_transform.row(sub_index) << sub_element_vertex.x, sub_element_vertex.y, sub_element_vertex.z;
      }

      jacobian_determinants.clear();
      auto elem_matrix = calcElementMatrix(element_type_, elem_transform, jacobian_determinants);
      auto b_matrix = makeStrainMatrix(element_count, elem_matrix);

      auto d_matrix = material_.GetStiffnessMatrix();

      Eigen::Matrix<precision, 6, 6> elementStiffnesMatrix =
          b_matrix.transpose() * d_matrix * b_matrix * jacobian_determinants.front() / 2.;

      for (uint32_t i = 0; i < element_count; ++i) {
        for (uint32_t j = 0; j < element_count; ++j) {
          const uint16_t global_index_i = element_inidices_[index + i];
          const uint16_t global_index_j = element_inidices_[index + j];

          // x coords
          triplets.push_back(T(DIM * global_index_i + 0, DIM * global_index_j + 0, elementStiffnesMatrix(DIM * i + 0, DIM * j + 0)));
          triplets.push_back(T(DIM * global_index_i + 0, DIM * global_index_j + 1, elementStiffnesMatrix(DIM * i + 0, DIM * j + 1)));

          // y coords
          triplets.push_back(T(DIM * global_index_i + 1, 2 * global_index_j + 0, elementStiffnesMatrix(DIM * i + 1, DIM * j + 0)));
          triplets.push_back(T(DIM * global_index_i + 1, 2 * global_index_j + 1, elementStiffnesMatrix(DIM * i + 1, DIM * j + 1)));
        }
      }

      index += element_count;
    }

    global_stiffness_matrix.setFromTriplets(triplets.begin(), triplets.end());
    return global_stiffness_matrix;
  }

  void ApplyConstraints(ElementMatrix &global_stiffnes_matrix) {
    std::vector<int> indicesToConstraint;

    for (const auto contraint : constraints_) {
      switch (DIM) {
        case 3:
          if (contraint.type & Constraint::UZ) indicesToConstraint.push_back(DIM * contraint.node + 2);
        case 2:
          if (contraint.type & Constraint::UY) indicesToConstraint.push_back(DIM * contraint.node + 1);
        case 1:
          if (contraint.type & Constraint::UX) indicesToConstraint.push_back(DIM * contraint.node + 0);
        default:
          break;
      }
    }

    for (int k = 0; k < global_stiffnes_matrix.outerSize(); ++k) {
      for (ElementMatrix::InnerIterator it(global_stiffnes_matrix, k); it; ++it) {
        for (auto index : indicesToConstraint) {
          if (it.row() == index || it.col() == index) {
            it.valueRef() = it.row() == it.col() ? 1.0f : 0.0f;
          }
        }
      }
    }
  }

 private:
  Loads buildLoadsVector(const std::vector<Load<DIM>> &loads) {
    Loads load_vector = Loads::Zero(elements_.size() * DIM);
    for (auto load : loads) {
      for (uint32_t i = 0; i < DIM; ++i) load_vector[load.node * DIM + i] = load.forces[i];
    }
    return load_vector;
  }

  // N matrix
  // [ Ni 0
  // [ 0  Ni
  // [ Ni Ni
  MatrixFixedRows<DIM> calcElementMatrix(std::shared_ptr<ScalarElement<DIM>> element_type, const MatrixFixedCols<DIM> &elem_transform,
                                         std::vector<precision> &jacobian_determinants) {
    MatrixFixedRows<DIM> elementMatrix = MatrixFixedRows<DIM>::Zero(DIM, element_type->GetElementCount());
    const std::vector<std::vector<precision>> &integration_points{{0.25, 0.25, 0.25}};
    jacobian_determinants.reserve(integration_points.size());

    for (const auto &ip : integration_points) {
      auto shape_function = element_type->CalcShape(ip);
      auto dshape = element_type->CalcDShape(ip);

      // build jacobian (d(x, y, z)/d(xi, eta, zeta))
      const MatrixDim<DIM> jacobian = dshape * elem_transform;
      const precision jacobian_det = jacobian.determinant();
      const MatrixDim<DIM> inverse_jacobian = jacobian.inverse();

      // element matrix
      elementMatrix += inverse_jacobian * dshape;
      jacobian_determinants.push_back(jacobian_det);
    }

    return elementMatrix;
  }

  // B matrix
  // [ Nix 0
  // [ 0   Niy
  // [ Niy Nix
  Eigen::Matrix<precision, 3, 6> makeStrainMatrix(const uint16_t element_count, const MatrixFixedRows<DIM> &elem_matrix) {
    Eigen::Matrix<precision, 3, 6> strain_matrix;
    for (int i = 0; i < element_count; ++i) {
      strain_matrix(0, 2 * i + 0) = elem_matrix(0, i);  // Nix
      strain_matrix(0, 2 * i + 1) = 0.0f;

      strain_matrix(1, 2 * i + 0) = 0.0f;
      strain_matrix(1, 2 * i + 1) = elem_matrix(1, i);  // Niy

      strain_matrix(2, 2 * i + 0) = elem_matrix(1, i);  // Niy
      strain_matrix(2, 2 * i + 1) = elem_matrix(0, i);  // Nix
    }
    return strain_matrix;
  }

  std::shared_ptr<ScalarElement<DIM>> element_type_;

  LinearMaterial<DIM> material_;
  std::vector<Vertex3> elements_;
  std::vector<uint16_t> element_inidices_;
  std::vector<ElementTransformations<DIM>> element_transformations_;
  std::vector<Constraint> constraints_;
  Loads loads_;
};

}  // namespace vulkan_fem

#endif  // model_h__
