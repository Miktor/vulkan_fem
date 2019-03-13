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
			model.BuildGlobalStiffnesMatrix();
		}
	};

} // vulkan_fem

#endif // solver_h__
