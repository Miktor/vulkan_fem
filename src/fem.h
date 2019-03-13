#ifndef fem_h__
#define fem_h__

#include <stdint.h>

#include <vector>

#include "Eigen/Dense"

namespace vulkan_fem
{
	using precision = float;

	template <size_t DIM = 3, typename Scalar = precision>
	using MatrixFixedRows = Eigen::Matrix<Scalar, DIM, Eigen::Dynamic>;

	template <size_t DIM = 3, typename Scalar = precision>
	using MatrixFixedCols = Eigen::Matrix<Scalar, Eigen::Dynamic, DIM>;

	template <size_t DIM = 3, typename Scalar = precision>
	using MatrixDim = Eigen::Matrix <Scalar, DIM, DIM>;

	template <size_t DIM = 3, typename Scalar = precision>
	using MatrixStrain = Eigen::Matrix <Scalar, DIM, DIM>;

	using MatrixX = Eigen::Matrix <precision, Eigen::Dynamic, Eigen::Dynamic>;
}

#endif // fem_h__
