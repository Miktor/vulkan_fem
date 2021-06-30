#pragma once

#include "Eigen/Dense"
#include "Eigen/Sparse"
#include <cstdint>
#include <vector>

namespace vulkan_fem {
using Precision = float;

template <size_t DIM = 3, typename Scalar = Precision>
using MatrixFixedRows = Eigen::Matrix<Scalar, DIM, Eigen::Dynamic>;

template <size_t DIM = 3, typename Scalar = Precision>
using MatrixFixedCols = Eigen::Matrix<Scalar, Eigen::Dynamic, DIM>;

template <size_t DIM = 3, typename Scalar = Precision>
using MatrixDim = Eigen::Matrix<Scalar, DIM, DIM>;

template <size_t DIM = 3, typename Scalar = Precision>
using MatrixStrain = Eigen::Matrix<Scalar, DIM, DIM>;

}  // namespace vulkan_fem

