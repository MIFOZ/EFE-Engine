#include "math/MathTypes.h"

namespace efe
{
	template <> const cMatrixf cMatrixf::Identity(	1,0,0,0,
													0,1,0,0,
													0,0,1,0,
													0,0,0,1);
	
	template <> const cMatrixf cMatrixf::Zero(	0,0,0,0,
												0,0,0,0,
												0,0,0,0,
												0,0,0,0);

	template <> const cVector3f cVector3f::Left(-1,0,0);
	template <> const cVector3f cVector3f::Right(1,0,0);
	template <> const cVector3f cVector3f::Up(0,1,0);
	template <> const cVector3f cVector3f::Down(0,-1,0);
	template <> const cVector3f cVector3f::Forward(0,0,1);
	template <> const cVector3f cVector3f::Back(0,0,-1);
}