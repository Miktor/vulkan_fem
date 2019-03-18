#ifndef model_h__
#define model_h__

#include "fem.h"
#include "material.h"
#include "elements.h"

namespace vulkan_fem
{
	struct Vertex3
	{
		precision x;
		precision y;
		precision z;
	};

	struct Constraint
	{
		enum Type
		{
			UX = 1 << 0,
			UY = 1 << 1,
			UZ = 1 << 2,
			UXY = UX | UY,
			UXZ = UX | UZ,
			UYZ = UY | UZ,
			UXYZ = UX | UY | UZ
		};

		uint32_t node;
		Type type;
	};

	template <uint32_t DIM>
	struct Load
	{
		uint32_t node;
		precision forces[DIM];
	};

	template <uint32_t DIM>
	struct DimentionHelper;

	template <>
	struct DimentionHelper<2>
	{
	};

	template <uint32_t DIM = 3>
	class Model
	{
	public:

		Model(std::shared_ptr<ScalarElement<DIM>> element_type,
			  const std::vector<Vertex3> &vertices,
			  const std::vector<uint16_t> &indices,
			  const std::vector<Constraint> &constraints,
			  const std::vector<Load<DIM>> &loads,
			  double E, double mu)
			: element_type_(element_type)
			, material_(E, mu)
			, elements_(vertices)
			, element_inidices_(indices)
			, constraints_(constraints)
			, loads_(loads)
		{

		}

		const std::vector<Vertex3>& GetVertices() const
		{
			return elements_;
		}

		const std::vector<uint16_t>& GetIndices() const
		{
			return element_inidices_;
		}

		MatrixX BuildGlobalStiffnesMatrix()
		{
			MatrixX global_stiffness_matrix = MatrixX::Zero(elements_.size() * DIM, elements_.size() * DIM);

			const uint32_t number_of_elements = element_inidices_.size() / DIM;

			const uint32_t element_count = element_type_->GetElementCount();
			const uint32_t order = element_type_->GetOrder();

			MatrixFixedCols<DIM> elem_transform(element_count, DIM);
			std::vector<precision> jacobian_determinants;

			for(uint32_t index = 0; index < element_inidices_.size();)
			{
				// put all vertex transforms into matrix
				for(uint16_t sub_index = 0; sub_index < element_count && index + sub_index < element_inidices_.size(); ++sub_index)
				{
					const uint16_t sub_element_index = element_inidices_[index + sub_index];
					const Vertex3& sub_element_vertex = elements_[sub_element_index];

					if (DIM == 2)
						elem_transform.row(sub_index) << sub_element_vertex.x, sub_element_vertex.y;
					else if (DIM == 3)
						elem_transform.row(sub_index) << sub_element_vertex.x, sub_element_vertex.y, sub_element_vertex.z;
				}
				
				jacobian_determinants.clear();
				auto elem_matrix = calcElementMatrix(element_type_, elem_transform, jacobian_determinants);
				auto b_matrix = makeStrainMatrix(element_count, elem_matrix);

				auto d_matrix = material_.GetStiffnessMatrix();

				Eigen::Matrix<precision, 6, 6> elementStiffnesMatrix = b_matrix.transpose() * d_matrix * b_matrix * jacobian_determinants.front() / 2.;

				for (uint32_t i = 0; i < element_count; ++i)
				{
					for (uint32_t j = 0; j < element_count; ++j)
					{
						const uint16_t global_index_i = element_inidices_[index + i];
						const uint16_t global_index_j = element_inidices_[index + j];

						// x coords
						global_stiffness_matrix(DIM * global_index_i + 0, DIM * global_index_j + 0)
							+= elementStiffnesMatrix(DIM * i + 0, DIM * j + 0);
						global_stiffness_matrix(DIM * global_index_i + 0, DIM * global_index_j + 1)
							+= elementStiffnesMatrix(DIM * i + 0, DIM * j + 1);

						// y coords
						global_stiffness_matrix(DIM * global_index_i + 1, 2 * global_index_j + 0)
							+= elementStiffnesMatrix(DIM * i + 1, DIM * j + 0);
						global_stiffness_matrix(DIM * global_index_i + 1, 2 * global_index_j + 1)
							+= elementStiffnesMatrix(DIM * i + 1, DIM * j + 1);
					}
				}
				

				index += element_count;
			}

			return global_stiffness_matrix;
		}

		void ApplyConstraints(MatrixX &global_stiffnes_matrix)
		{
			std::vector<int> indicesToConstraint;

			for (const auto it : constraints_)
			{
				switch (DIM)
				{
					case 3:
						if (it->type & Constraint::UZ)
						{
							indicesToConstraint.push_back(DIM * it->node + 2);
						}
					case 2:
						if (it->type & Constraint::UY)
						{
							indicesToConstraint.push_back(DIM * it->node + 1);
						}
					case 1:
						if (it->type & Constraint::UX)
						{
							indicesToConstraint.push_back(DIM * it->node + 0);
						}
					default:
						break;
				}
			}
		}

	private:

		// N matrix
		// [ Ni 0 
		// [ 0  Ni
		// [ Ni Ni
		MatrixFixedRows<DIM> calcElementMatrix(
			std::shared_ptr<ScalarElement<DIM>> element_type,
			const MatrixFixedCols<DIM> &elem_transform,
			std::vector<precision> &jacobian_determinants)
		{
			MatrixFixedRows<DIM> elementMatrix = MatrixFixedRows<DIM>::Zero(DIM, element_type->GetElementCount());
			const std::vector<std::vector<precision>> & integration_points {{0.25, 0.25, 0.25}};
			jacobian_determinants.reserve(integration_points.size());

			for (const auto& ip : integration_points)
			{
				auto shape_function = element_type->CalcShape(ip);
				auto dshape = element_type->CalcDShape(ip);

				// build jacobian (d(x, y, z)/d(xi, eta, zeta))
				const MatrixDim<DIM> jacobian = dshape * elem_transform;
				const precision jacobian_det = jacobian.determinant();
				const MatrixDim<DIM> inverse_jacobian = jacobian.inverse();

				//element matrix 
				elementMatrix += inverse_jacobian * dshape;
				jacobian_determinants.push_back(jacobian_det);
			}

			return elementMatrix;
		}

		// B matrix
		// [ Nix 0 
		// [ 0   Niy
		// [ Niy Nix
		Eigen::Matrix<precision, 3, 6> makeStrainMatrix(const uint16_t element_count, const MatrixFixedRows<DIM> &elem_matrix)
		{
			Eigen::Matrix<precision, 3, 6> strain_matrix;
			for (int i = 0; i < element_count; ++i)
			{
				strain_matrix(0, 2 * i + 0) = elem_matrix(0, i); // Nix
				strain_matrix(0, 2 * i + 1) = 0.0f;

				strain_matrix(1, 2 * i + 0) = 0.0f;
				strain_matrix(1, 2 * i + 1) = elem_matrix(1, i); // Niy

				strain_matrix(2, 2 * i + 0) = elem_matrix(1, i); // Niy
				strain_matrix(2, 2 * i + 1) = elem_matrix(0, i); // Nix
			}
			return strain_matrix;
		}

		std::shared_ptr<ScalarElement<DIM>> element_type_;

		LinearMaterial<DIM> material_;
		std::vector<Vertex3> elements_;
		std::vector<uint16_t> element_inidices_;
		std::vector<ElementTransformations<DIM>> element_transformations_;
		std::vector<Constraint> constraints_;
		std::vector<Load<DIM>> loads_;
	};
} // vulkan_fem

#endif // model_h__
