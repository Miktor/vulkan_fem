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


	template <uint32_t DIM = 3>
	class Model
	{
	public:

		Model(std::shared_ptr<ScalarElement<DIM>> element_type,
			  const std::vector<Vertex3> &vertices,
			  const std::vector<uint16_t> &indices,
			  double E, double mu)
			: element_type_(element_type)
			, material_(E, mu)
			, elements_(vertices)
			, element_inidices_(indices)
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

		void BuildGlobalStiffnesMatrix()
		{
			MatrixX global_stiffness_matrix = MatrixX(elements_.size(), elements_.size());

			const uint32_t number_of_elements = element_inidices_.size() / DIM;

			const uint32_t ndof = element_type_->GetNDof();
			const uint32_t order = element_type_->GetOrder();

			MatrixFixedCols<DIM> elem_transform(ndof, DIM);
			std::vector<precision> jacobian_determinants;

			for(uint32_t index = 0; index < element_inidices_.size();)
			{
				for(uint16_t sub_index = 0; sub_index < ndof && index + sub_index < element_inidices_.size(); ++sub_index)
				{
					const uint16_t sub_element_index = element_inidices_[index + sub_index];
					const Vertex3& sub_element_vertex = elements_[sub_element_index];

					if (DIM == 2)
						elem_transform.row(sub_index) << sub_element_vertex.x, sub_element_vertex.y;
					else if (DIM == 3)
						elem_transform.row(sub_index) << sub_element_vertex.x, sub_element_vertex.y, sub_element_vertex.z;
				}

				index += ndof;
				
				jacobian_determinants.clear();
				auto elem_matrix = element_type_->CalcElementMatrix(elem_transform, jacobian_determinants);
				auto b_matrix = makeStrainMatrix(ndof, elem_matrix);

				auto d_matrix = material_.GetStiffnessMatrix();

				auto elementStiffnesMatrix = b_matrix.transpose() * d_matrix * b_matrix * jacobian_determinants.front() / 2.;

				for (int i = 0; i < ndof; i++)
				{
					for (int j = 0; j < ndof; j++)
					{
						Eigen::Triplet<float> trplt11(2 * nodesIds[i] + 0, 2 * nodesIds[j] + 0, K(2 * i + 0, 2 * j + 0));
						Eigen::Triplet<float> trplt12(2 * nodesIds[i] + 0, 2 * nodesIds[j] + 1, K(2 * i + 0, 2 * j + 1));
						Eigen::Triplet<float> trplt21(2 * nodesIds[i] + 1, 2 * nodesIds[j] + 0, K(2 * i + 1, 2 * j + 0));
						Eigen::Triplet<float> trplt22(2 * nodesIds[i] + 1, 2 * nodesIds[j] + 1, K(2 * i + 1, 2 * j + 1));
					}
				}

			}

		}

	private:


		Eigen::Matrix<precision, 3, 6>  makeStrainMatrix(const uint16_t ndof, const MatrixFixedRows<DIM> &elem_matrix)
		{
			Eigen::Matrix<precision, 3, 6> strain_matrix;
			for (int i = 0; i < 3; ++i)
			{
				strain_matrix(0, 2 * i + 0) = elem_matrix(0, i);
				strain_matrix(0, 2 * i + 1) = 0.0f;
				strain_matrix(1, 2 * i + 0) = 0.0f;
				strain_matrix(1, 2 * i + 1) = elem_matrix(1, i);
				strain_matrix(2, 2 * i + 0) = elem_matrix(1, i);
				strain_matrix(2, 2 * i + 1) = elem_matrix(0, i);
			}
			return strain_matrix;
		}

		std::shared_ptr<ScalarElement<DIM>> element_type_;

		LinearMaterial<DIM> material_;
		std::vector<Vertex3> elements_;
		std::vector<uint16_t> element_inidices_;
		std::vector<ElementTransformations<DIM>> element_transformations_;
	};
} // vulkan_fem

#endif // model_h__
