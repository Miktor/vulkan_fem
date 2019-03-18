#ifndef solver_h__
#define solver_h__

#include "fem.h"

namespace vulkan_fem
{
	template <uint32_t DIM = 3>
	class Solver
	{
	public:

		void Solve(Model<DIM> & model)
		{
			auto global_stiffnes_matrix = model.BuildGlobalStiffnesMatrix(); // K_global

			model.ApplyConstraints(global_stiffnes_matrix);

			Eigen::SimplicialLDLT<decltype(global_stiffnes_matrix)> solver(global_stiffnes_matrix);
			Eigen::VectorXf displacements = solver.solve(loads);
		}
	};

} // vulkan_fem

#endif // solver_h__
