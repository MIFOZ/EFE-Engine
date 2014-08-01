#ifndef EFE_RENDER_TASK_H
#define EFE_RENDER_TASK_H

#include "math/Math.h"

namespace efe
{
	class cRenderTask
	{
	public:
		cRenderTask();
		virtual ~cRenderTask();

	protected:
		cColor m_Color;

		cMatrixf m_mtxView;
		cMatrixf m_mtxProj;
	};
};

#endif