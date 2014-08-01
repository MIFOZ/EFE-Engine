#include "gui/GuiMaterialBasicTypes.h"

#include "graphics/LowLevelGraphics.h"

namespace efe
{
	void cGuiMaterial_Diffuse::BeforeRender()
	{
		
	}

	void cGuiMaterial_Diffuse::AfterRender()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// ALPHA
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiMaterial_Alpha::cGuiMaterial_Alpha(iLowLevelGraphics *a_pLowLevelGraphics)
		: iGuiMaterial("Alpha", a_pLowLevelGraphics)
	{
		m_pBS = m_pLowLevelGraphics->CreateBlendState(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
	}

	cGuiMaterial_Alpha::~cGuiMaterial_Alpha()
	{
		efeDelete(m_pBS);
	}

	void cGuiMaterial_Alpha::BeforeRender()
	{
		m_pLowLevelGraphics->SetBlendState(m_pBS);
	}

	void cGuiMaterial_Alpha::AfterRender()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// FONT NORMAL
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiMaterial_FontNormal::cGuiMaterial_FontNormal(iLowLevelGraphics *a_pLowLevelGraphics)
		: iGuiMaterial("FontNormal", a_pLowLevelGraphics)
	{
		m_pBS = m_pLowLevelGraphics->CreateBlendState(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
	}

	cGuiMaterial_FontNormal::~cGuiMaterial_FontNormal()
	{
		efeDelete(m_pBS);
	}

	void cGuiMaterial_FontNormal::BeforeRender()
	{
		m_pLowLevelGraphics->SetBlendState(m_pBS);
	}

	void cGuiMaterial_FontNormal::AfterRender()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// ADDITIVE
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cGuiMaterial_Additive::BeforeRender()
	{
		
	}

	void cGuiMaterial_Additive::AfterRender()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// MODULATIVE
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiMaterial_Modulative::cGuiMaterial_Modulative(iLowLevelGraphics *a_pLowLevelGraphics)
		: iGuiMaterial("Modulative", a_pLowLevelGraphics)
	{
		m_pBS = m_pLowLevelGraphics->CreateBlendState(eBlendFunc_DestColor, eBlendFunc_Zero, eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha, eBlendOp_Add, eBlendOp_Add);
	}

	cGuiMaterial_Modulative::~cGuiMaterial_Modulative()
	{
		efeDelete(m_pBS);
	}

	void cGuiMaterial_Modulative::BeforeRender()
	{
		m_pLowLevelGraphics->SetBlendState(m_pBS);
	}

	void cGuiMaterial_Modulative::AfterRender()
	{
		
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// INVERSE
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiMaterial_Inverse::cGuiMaterial_Inverse(iLowLevelGraphics *a_pLowLevelGraphics)
		: iGuiMaterial("Inverse", a_pLowLevelGraphics)
	{
		m_pBS = m_pLowLevelGraphics->CreateBlendState(eBlendFunc_OneMinusDestColor, eBlendFunc_Zero, eBlendFunc_SrcAlpha, eBlendFunc_Zero, eBlendOp_Add, eBlendOp_Add);
	}

	cGuiMaterial_Inverse::~cGuiMaterial_Inverse()
	{
		efeDelete(m_pBS);
	}

	void cGuiMaterial_Inverse::BeforeRender()
	{
		m_pLowLevelGraphics->SetBlendState(m_pBS);
	}

	void cGuiMaterial_Inverse::AfterRender()
	{
		
	}
}