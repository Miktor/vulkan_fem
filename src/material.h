#ifndef Material_h__
#define Material_h__

#include "fem.h"

#include <Eigen/Dense>

namespace vulkan_fem
{
	class Material
	{
	public:

	};

	template <uint32_t DIM>
	class LinearMaterial {};

	template <>
	class LinearMaterial<3> : public Material
	{
	public:

		Eigen::Matrix<precision, 6, 6> stiffnesMatrix;

		LinearMaterial(double E, double nu)
		{
			const precision lambda = E * nu / (1. + nu) / (1. + 2. * nu);
			const precision mu = E / 2 / (1. + nu);

			const precision c1 = lambda + 2. * mu;

			stiffnesMatrix <<
				c1, lambda, lambda, 0, 0, 0,
				lambda, c1, lambda, 0, 0, 0,
				lambda, lambda, c1, 0, 0, 0,
				0, 0, 0, mu, 0, 0,
				0, 0, 0, 0, mu, 0,
				0, 0, 0, 0, 0, mu;
		}

		virtual Eigen::Matrix<precision, 6, 6> GetStiffnessMatrix()
		{
			return stiffnesMatrix;
		}

	private:

	};

	template <>
	class LinearMaterial<2> : public Material
	{
	public:

		Eigen::Matrix<precision, 3, 3> stiffnesMatrix;

		LinearMaterial(double E, double nu)
		{
			stiffnesMatrix <<
				1.0f, nu, 0.0f,
				nu, 1.0, 0.0f,
				0.0f, 0.0f, (1.0f - nu) / 2.0f;

			stiffnesMatrix *= E / (1.0f - pow(nu, 2.0f));
		}

		virtual Eigen::Matrix<precision, 3, 3> GetStiffnessMatrix()
		{
			return stiffnesMatrix;
		}

	private:

	};
}
#endif // Material_h__
