#ifndef EFE_GUI_MATERIAL_H
#define EFE_GUI_MATERIAL_H

#include "system/SystemTypes.h"

namespace efe
{
	class iLowLevelGraphics;

	class iGuiMaterial
	{
	public:
		iGuiMaterial(const tString &a_sName, iLowLevelGraphics *a_pLowLevelGraphics) : 
		  m_sName(a_sName), m_pLowLevelGraphics(a_pLowLevelGraphics){}
		  virtual ~iGuiMaterial(){}

		virtual void BeforeRender()=0;
		virtual void AfterRender()=0;

		const tString &GetName(){return m_sName;}

	protected:
		tString m_sName;
		iLowLevelGraphics *m_pLowLevelGraphics;
	};
};
#endif