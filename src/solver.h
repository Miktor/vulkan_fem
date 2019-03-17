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

		}
	};

} // vulkan_fem

#endif // solver_h__
