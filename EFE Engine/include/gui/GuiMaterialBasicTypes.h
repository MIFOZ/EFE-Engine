#ifndef EFE_GUI_MATERIAL_BASIC_TYPES_H
#define EFE_GUI_MATERIAL_BASIC_TYPES_H

#include "gui/GuiMaterial.h"

namespace efe
{
	class iBlendState;

	class cGuiMaterial_Diffuse : public iGuiMaterial
	{
	public:
		cGuiMaterial_Diffuse(iLowLevelGraphics *a_pLowLevelGraphics) : iGuiMaterial("Diffuse", a_pLowLevelGraphics){}
		~cGuiMaterial_Diffuse(){}

		void BeforeRender();
		void AfterRender();
	};

	class cGuiMaterial_Alpha : public iGuiMaterial
	{
	public:
		cGuiMaterial_Alpha(iLowLevelGraphics *a_pLowLevelGraphics);
		~cGuiMaterial_Alpha();

		void BeforeRender();
		void AfterRender();
	private:
		iBlendState *m_pBS;
	};

	class cGuiMaterial_FontNormal : public iGuiMaterial
	{
	public:
		cGuiMaterial_FontNormal(iLowLevelGraphics *a_pLowLevelGraphics);
		~cGuiMaterial_FontNormal();

		void BeforeRender();
		void AfterRender();

	private:
		iBlendState *m_pBS;
	};

	class cGuiMaterial_Additive : public iGuiMaterial
	{
	public:
		cGuiMaterial_Additive(iLowLevelGraphics *a_pLowLevelGraphics) : iGuiMaterial("Additive", a_pLowLevelGraphics){}
		~cGuiMaterial_Additive(){}

		void BeforeRender();
		void AfterRender();
	};

	class cGuiMaterial_Modulative : public iGuiMaterial
	{
	public:
		cGuiMaterial_Modulative(iLowLevelGraphics *a_pLowLevelGraphics);
		~cGuiMaterial_Modulative();

		void BeforeRender();
		void AfterRender();

	private:
		iBlendState *m_pBS;
	};

	class cGuiMaterial_Inverse : public iGuiMaterial
	{
	public:
		cGuiMaterial_Inverse(iLowLevelGraphics *a_pLowLevelGraphics);
		~cGuiMaterial_Inverse();

		void BeforeRender();
		void AfterRender();

	private:
		iBlendState *m_pBS;
	};
};
#endif