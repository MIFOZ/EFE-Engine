#include "graphics/RenderState.h"
#include "graphics/Renderer3D.h"

#include "graphics/Material.h"
#include "graphics/VertexBuffer.h"

#include "graphics/LowLevelGraphics.h"


#include "scene/Camera3D.h"
#include "scene/PortalContainer.h"
#include "math/Math.h"

namespace efe
{
	template<class T>
	static int GetCompareVal(T a, T b)
	{
		if (a==b) return 0;
		else return a < b ? -1 : 1;
	}

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	int iRenderState::Compare(const iRenderState *a_pState) const
	{
		switch (m_Type)
		{
		case efe::eRenderStateType_Sector:			return CompareSector(a_pState);
		case efe::eRenderStateType_Pass:			return ComparePass(a_pState);
		case efe::eRenderStateType_DepthTest:		return CompareDepthTest(a_pState);
		case efe::eRenderStateType_Depth:			return CompareDepth(a_pState);
		case efe::eRenderStateType_AlphaMode:		return CompareAlpha(a_pState);
		case efe::eRenderStateType_BlendMode:		return CompareBlend(a_pState);
		case efe::eRenderStateType_VertexProgram:	return CompareVtxProg(a_pState);
		case efe::eRenderStateType_PixelProgram:	return ComparePxlProg(a_pState);
		case efe::eRenderStateType_Texture:			return CompareTexture(a_pState);
		case efe::eRenderStateType_VertexBuffer:	return CompareVtxBuff(a_pState);
		case efe::eRenderStateType_Matrix:			return CompareMatrix(a_pState);
		case efe::eRenderStateType_Render:			return CompareRender(a_pState);
		}

		return 0;
	}

	void iRenderState::SetMode(cRenderSettings *a_pSettings)
	{
		switch(m_Type)
		{
		case eRenderStateType_Sector:			SetSectorMode(a_pSettings);break;
		case eRenderStateType_Pass:				SetPassMode(a_pSettings); break;
		case eRenderStateType_DepthTest:		SetDepthTestMode(a_pSettings); break;			
		case eRenderStateType_Depth:			SetDepthMode(a_pSettings); break;
		case eRenderStateType_AlphaMode:		SetAlphaMode(a_pSettings); break;
		case eRenderStateType_BlendMode:		SetBlendMode(a_pSettings); break;
		case eRenderStateType_VertexProgram:	SetVtxProgMode(a_pSettings); break;
		case eRenderStateType_PixelProgram:		SetPxlProgMode(a_pSettings); break;
		case eRenderStateType_Texture:			SetTextureMode(a_pSettings); break;
		case eRenderStateType_VertexBuffer:		SetVtxBuffMode(a_pSettings); break;
		case eRenderStateType_Matrix:			SetMatrixMode(a_pSettings); break;
		case eRenderStateType_Render:			SetRenderMode(a_pSettings); break;
		}
	}

	void iRenderState::Set(const iRenderState *a_pState)
	{
		m_Type = a_pState->m_Type;
		switch(m_Type)
		{
		case eRenderStateType_Sector:			m_pSector = a_pState->m_pSector;
												break;
		case eRenderStateType_Pass:				m_lPass = a_pState->m_lPass;
												break;
		case eRenderStateType_DepthTest:		m_bDepthTest = a_pState->m_bDepthTest;
												break;
		case eRenderStateType_Depth:			m_fZ = a_pState->m_fZ;
												break;
		case eRenderStateType_AlphaMode:		m_AlphaMode = a_pState->m_AlphaMode;
												break;
		case eRenderStateType_BlendMode:		m_BlendMode = a_pState->m_BlendMode;
												break;
		case eRenderStateType_VertexProgram:	m_pVtxProgram = a_pState->m_pVtxProgram;
												m_pVtxProgramSetup = a_pState->m_pVtxProgramSetup;
												m_bUsesEye = a_pState->m_bUsesEye;
												m_bUsesLight = a_pState->m_bUsesLight;
												m_pLight = a_pState->m_pLight;
												break;
		case eRenderStateType_PixelProgram:		m_pPxlProgram = a_pState->m_pPxlProgram;
												m_pPxlProgramSetup = a_pState->m_pPxlProgramSetup;
												break;
		case eRenderStateType_Matrix:			m_pModelMatrix = a_pState->m_pModelMatrix;
												m_pInvModelMatrix = a_pState->m_pInvModelMatrix;
												m_vScale = a_pState->m_vScale;
												break;
		case eRenderStateType_Texture:			for (int i=0;i<MAX_TEXTUREUNITS;i++)
												{
													m_pTexture[i] = a_pState->m_pTexture[i];
													m_pTextureParams[i] = a_pState->m_pTextureParams[i];
												}
												break;
		case eRenderStateType_VertexBuffer:		m_pVtxBuffer = a_pState->m_pVtxBuffer;
												break;
		case eRenderStateType_Render:			m_pObject = a_pState->m_pObject;
												m_pRenderEffect = a_pState->m_pRenderEffect;
												break;
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// COMPARE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	int iRenderState::CompareSector( const iRenderState *a_pSettings ) const
	{
		return -1;
	}

	//--------------------------------------------------------------

	int iRenderState::ComparePass( const iRenderState *a_pState ) const
	{
		return m_lPass < a_pState->m_lPass;
	}

	//--------------------------------------------------------------

	int iRenderState::CompareDepthTest(const iRenderState *a_pState)const
	{
		return (int)m_bDepthTest < (int)a_pState->m_bDepthTest;
	}

	//--------------------------------------------------------------

	int iRenderState::CompareDepth(const iRenderState *a_pState)const
	{
		if (std::abs(m_fZ - a_pState->m_fZ) < 0.00001f) return 0;
		else return m_fZ < a_pState->m_fZ ? 1 : -1;
	}

	//--------------------------------------------------------------

	int iRenderState::CompareAlpha(const iRenderState *a_pState)const
	{
		return GetCompareVal((int)m_AlphaMode, (int)a_pState->m_AlphaMode);
	}

	//--------------------------------------------------------------

	int iRenderState::CompareBlend(const iRenderState *a_pState)const
	{
		return GetCompareVal((int)m_BlendMode, (int)a_pState->m_BlendMode);
	}

	//--------------------------------------------------------------

	int iRenderState::CompareVtxProg(const iRenderState *a_pState)const
	{
		return GetCompareVal(m_pVtxProgram, a_pState->m_pVtxProgram);
	}

	//--------------------------------------------------------------

	int iRenderState::ComparePxlProg(const iRenderState *a_pState)const
	{
		return GetCompareVal(m_pPxlProgram, a_pState->m_pPxlProgram);
	}

	//--------------------------------------------------------------

	int iRenderState::CompareTexture(const iRenderState *a_pState)const
	{
		for (int i = 0; i < MAX_TEXTUREUNITS; i++)
		{
			if (m_pTexture[i] != a_pState->m_pTexture[i])
				return GetCompareVal(m_pTexture[i], a_pState->m_pTexture[i]);
		}
		return GetCompareVal(m_pTexture[MAX_TEXTUREUNITS-1], a_pState->m_pTexture[MAX_TEXTUREUNITS-1]);
	}

	//--------------------------------------------------------------

	int iRenderState::CompareVtxBuff(const iRenderState *a_pState)const
	{
		return GetCompareVal(m_pVtxBuffer, a_pState->m_pVtxBuffer);
	}

	//--------------------------------------------------------------

	int iRenderState::CompareMatrix(const iRenderState *a_pState)const
	{
		return GetCompareVal(m_pModelMatrix, a_pState->m_pModelMatrix);
	}

	//--------------------------------------------------------------

	int iRenderState::CompareRender(const iRenderState *a_pState)const
	{
		return GetCompareVal(m_pObject, a_pState->m_pObject);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// SET MODE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iRenderState::SetSectorMode(cRenderSettings *a_pSettings)
	{
		a_pSettings->m_pPixelProgram = NULL;
	}

	//--------------------------------------------------------------

	void iRenderState::SetPassMode(cRenderSettings *a_pSettings)
	{
		if (a_pSettings->m_bLog) Log("Pass: %d\n", m_lPass);
	}

	//--------------------------------------------------------------

	void iRenderState::SetDepthTestMode(cRenderSettings *a_pSettings)
	{
		if (a_pSettings->m_bDepthTest != m_bDepthTest)
		{
			a_pSettings->m_pLowLevel->SetClearDepthActive(m_bDepthTest);
			a_pSettings->m_bDepthTest = m_bDepthTest;
		}
	}

	//--------------------------------------------------------------

	void iRenderState::SetDepthMode(cRenderSettings *a_pSettings)
	{

	}

	//--------------------------------------------------------------

	void iRenderState::SetAlphaMode(cRenderSettings *a_pSettings)
	{
		if (m_AlphaMode != a_pSettings->m_AlphaMode)
		{
			if (a_pSettings->m_bLog) Log("Setting alpha: ");
			a_pSettings->m_AlphaMode = m_AlphaMode;

			if (m_AlphaMode == eMaterialAlphaMode_Solid)
			{
				//a_pSettings->m_pLowLevel->seta
				if (a_pSettings->m_bLog) Log("Solid");
			}
			else
			{
				if (a_pSettings->m_bLog) Log("Trans");
			}
		}

		if (a_pSettings->m_bLog) Log("\n");
	}

	//--------------------------------------------------------------

	void iRenderState::SetBlendMode(cRenderSettings *a_pSettings)
	{
		if (m_BlendMode != a_pSettings->m_BlendMode)
		{
			if (a_pSettings->m_bLog) Log("Setting blend mode: ");
			a_pSettings->m_BlendMode = m_BlendMode;

			if (m_BlendMode == eMaterialBlendMode_None)
			{
				a_pSettings->m_pLowLevel->SetBlendActive(false);
				if (a_pSettings->m_bLog) Log("None");
			}
			else
			{
				a_pSettings->m_pLowLevel->SetBlendActive(true);

				switch (m_BlendMode)
				{
				case efe::eMaterialBlendMode_Add:
					a_pSettings->m_pLowLevel->SetBlendFunc(eBlendFunc_One, eBlendFunc_One);
					if (a_pSettings->m_bLog) Log("Add");
					break;
				case efe::eMaterialBlendMode_Mul:
					a_pSettings->m_pLowLevel->SetBlendFunc(eBlendFunc_Zero, eBlendFunc_SrcColor);
					if (a_pSettings->m_bLog) Log("Mul");
					break;
				case efe::eMaterialBlendMode_MulX2:
					a_pSettings->m_pLowLevel->SetBlendFunc(eBlendFunc_DestColor, eBlendFunc_SrcColor);
					if (a_pSettings->m_bLog) Log("MulX2");
					break;
				case efe::eMaterialBlendMode_Replace:
					a_pSettings->m_pLowLevel->SetBlendFunc(eBlendFunc_One, eBlendFunc_Zero);
					if (a_pSettings->m_bLog) Log("Replace");
					break;
				case efe::eMaterialBlendMode_Alpha:
					a_pSettings->m_pLowLevel->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_OneMinusSrcAlpha);
					if (a_pSettings->m_bLog) Log("Alpha");
					break;
				case efe::eMaterialBlendMode_DestAlphaAdd:
					a_pSettings->m_pLowLevel->SetBlendFunc(eBlendFunc_DestAlpha, eBlendFunc_One);
					if (a_pSettings->m_bLog) Log("DestAlphaAdd");
					break;
				}
			}

			if (a_pSettings->m_bLog) Log("\n");
		}
	}

	//--------------------------------------------------------------

	void iRenderState::SetVtxProgMode(cRenderSettings *a_pSettings)
	{
		/*m_pVtxProgram->Bind();*/
		//a_pSettings->m_pLowLevel->s

// 		if (m_pVtxProgram != a_pSettings->m_pVertexProgram)
// 		{
// 			if (a_pSettings->m_bLog)
// 			{
// 				if (m_pVtxProgram)
// 					Log("Setting vertex program: '%s'/%d ", m_pVtxProgram->GetName().c_str(),
// 					(size_t)m_pVtxProgram);
// 				else
// 					Log("Setting vertex program: NULL ");
// 			}
// 
// 			if (m_pVtxProgram == NULL && a_pSettings->m_pVertexProgram)
// 			{
// 				a_pSettings->m_pVertexProgram->UnBind();
// 				if (a_pSettings->m_bLog) Log("Unbinding old ");
// 			}
// 			a_pSettings->m_pVertexProgram = m_pVtxProgram;
// 
// 			if (m_pVtxProgram)
// 			{
// 				if (a_pSettings->m_bLog) Log("Binding new ");
// 				m_pVtxProgram->Bind();
// 
// 				if (m_pVtxProgramSetup)
// 				{
// 					if (a_pSettings->m_bLog) Log("Custom setup %d ", m_pVtxProgram);
// 					m_pVtxProgramSetup->SetUp(m_pVtxProgram, a_pSettings);
// 				}
// 				a_pSettings->m_pVtxProgramSetup = m_pVtxProgramSetup;
// 
// 				a_pSettings->m_bMatrixWasNULL = false;
// 
// 				if (m_bUsesLight)
// 				{
// 
// 				}
// 				else
// 					a_pSettings->m_pLight = NULL;
// 
// 				a_pSettings->m_bUsesLight = m_bUsesLight;
// 				a_pSettings->m_bUsesEye = m_bUsesEye;
// 			}
// 			if (a_pSettings->m_bLog) Log("\n");
// 		}
// 		else
// 		{
// 			if (a_pSettings->m_pVertexProgram && m_bUsesEye && m_pLight != a_pSettings->m_pLight)
// 			{
// 				if (a_pSettings->m_bLog) Log("Setting new light properties\n");
// 
// 				a_pSettings->m_pLight = m_pLight;
// 			}
// 		}
	}

	//--------------------------------------------------------------

	void iRenderState::SetPxlProgMode(cRenderSettings *a_pSettings)
	{
		/*m_pPxlProgram->Bind();*/
// 		if (m_pPxlProgram != a_pSettings->m_pPixelProgram)
// 		{
// 			if (a_pSettings->m_bLog)
// 			{
// 				if (m_pPxlProgram)
// 					Log("Setting pixel program: '%s'/%d ", m_pPxlProgram->GetName().c_str(),
// 					(size_t)m_pPxlProgram);
// 				else
// 					Log("Setting pixel program: NULL");
// 			}
// 
// 			if (a_pSettings->m_pPixelProgram) a_pSettings->m_pPixelProgram->UnBind();
// 
// 			a_pSettings->m_pPixelProgram = m_pPxlProgram;
// 
// 			if (m_pPxlProgram)
// 			{
// 				if (a_pSettings->m_bLog) Log("Binding new ");
// 				m_pPxlProgram->Bind();
// 
// 				if (m_pPxlProgramSetup) m_pPxlProgramSetup->SetUp(m_pPxlProgram, a_pSettings);
// 			}
// 
// 			if (a_pSettings->m_bLog) Log("\n");
// 		}
	}

	//--------------------------------------------------------------

	void iRenderState::SetTextureMode(cRenderSettings *a_pSettings)
	{
		for (int i=0; i<(int)eMaterialTexture_LastEnum; i++)
		{
			if (m_pTextureParams[i]->m_pTexture)
			{
				cMaterialTextureParam *pParam = m_pTextureParams[i];
				switch (i)
				{
				case eMaterialTexture_Diffuse:
					a_pSettings->m_pLowLevel->SetTexture("diffuseMap", m_pTexture[i]);
					a_pSettings->m_pLowLevel->SetTextureSamplerParams("diffuseSampler", pParam->m_WrapS, pParam->m_WrapT, pParam->m_WrapR, pParam->m_Filter, pParam->m_fAnisotropyDegree);
					break;
				case eMaterialTexture_NMap: a_pSettings->m_pLowLevel->SetTexture("normalMap", m_pTexture[i]); break;
				default:
					break;
				}
			}
		}

// 		for (int i = 0; i < MAX_TEXTUREUNITS; i++)
// 		{
// 			if (a_pSettings->m_pTexture[i] != m_pTexture[i])
// 			{
// 				if (a_pSettings->m_bLog)
// 				{
// 					if (m_pTexture[i] == NULL)
// 						Log("Setting texture: %d / %d : NULL\n", i, (size_t)m_pTexture[i]);
// 					else
// 						Log("Setting texture: %d / %d : '%s'\n", i, (size_t)m_pTexture[i],
// 						m_pTexture[i]->GetName().c_str());
// 				}
// 			}
// 		}
	}

	//--------------------------------------------------------------

	void iRenderState::SetSamplerMode(cRenderSettings *a_pSettings)
	{
		if (m_TextureAddressMode[0] != a_pSettings->m_TextureAddressMode[0])
		{
			//a_pSettings->m_pLowLevel->SetTextureAddressMode(m_TextureAddressMode[0]);
			if (a_pSettings->m_bLog)
			{
				tString sText = "Setting texture address mode: ";
				switch (m_TextureAddressMode[0])
				{
				case eTextureAddressMode_Wrap: sText += "Wrap"; break;
				case eTextureAddressMode_Clamp: sText += "Clamp"; break;
				case eTextureAddressMode_Mirror: sText += "Mirror"; break;
				case eTextureAddressMode_Border: sText += "Border"; break;
				default: sText += "Yasno"; break;
				}
				sText += "\n";
				Log(sText.c_str());
			}

			a_pSettings->m_TextureAddressMode[0] = m_TextureAddressMode[0];
		}
		if (m_TextureFilter[0] != a_pSettings->m_TextureFilter[0])
		{
			//a_pSettings->m_pLowLevel->SetTextureFilter(m_TextureFilter[0]);
			if (a_pSettings->m_bLog)
			{
				tString sText = "Setting filter: ";
				switch (m_TextureFilter[0])
				{
				case eTextureFilter_Nearest: sText += "Nearest"; break;
				case eTextureFilter_Linear: sText += "Linear"; break;
				case eTextureFilter_Bilinear: sText += "Bilinear"; break;
				case eTextureFilter_Trilinear: sText += "Trilinear"; break;
				case eTextureFilter_BilinearAniso: sText += "BilinearAniso"; break;
				case eTextureFilter_TrilinearAniso: sText += "TrilinearAniso"; break;
				default: sText += "Yasno";
				}
				sText += "\n";
				Log(sText.c_str());
			}

			a_pSettings->m_TextureFilter[0] = m_TextureFilter[0];
		}
	}

	//--------------------------------------------------------------

	void iRenderState::SetVtxBuffMode(cRenderSettings *a_pSettings)
	{
		if (m_pVtxBuffer != a_pSettings->m_pVtxBuffer)
		{
			if (a_pSettings->m_bLog) Log("Setting vertex buffer: %d\n", (size_t)m_pVtxBuffer);
			//if (a_pSettings->m_pVtxBuffer) a_pSettings->m_pVtxBuffer->unbi
			a_pSettings->m_pVtxBuffer = m_pVtxBuffer;

			if (m_pVtxBuffer)
			{
				//m_pVtxBuffer->Bind();
			}
		}
	}

	//--------------------------------------------------------------

	void iRenderState::SetMatrixMode(cRenderSettings *a_pSettings)
	{
		a_pSettings->m_pLowLevel->SetMatrix(eMatrix_Model, *m_pModelMatrix);
		a_pSettings->m_pLowLevel->SetMatrix(eMatrix_View, a_pSettings->m_pCamera->GetViewMatrix());
		a_pSettings->m_pLowLevel->SetMatrix(eMatrix_Projection, a_pSettings->m_pCamera->GetProjectionMatrix());

		if (m_pModelMatrix)
		{
			if (a_pSettings->m_bLog) Log("Setting model matrix: %s\n", cMath::MatrixToChar(*m_pModelMatrix));

			a_pSettings->m_bMatrixWasNULL = false;
		}
		else
		{
			if (a_pSettings->m_bLog) Log("Setting model matrix: NULL\n");

			a_pSettings->m_bMatrixWasNULL = true;
		}
	}

	//--------------------------------------------------------------

	void iRenderState::SetRenderMode(cRenderSettings *a_pSettings)
	{
		if (a_pSettings->m_bLog) Log("Drawing\n-----------------\n");

		if (a_pSettings->m_DebugFlags & eRendererDebugFlag_DrawLines)
		{
			a_pSettings->m_pVtxBuffer->Draw(m_pRenderEffect, eVertexBufferDrawType_LineStrip);
		}
		else
		{
			a_pSettings->m_pVtxBuffer->Draw(m_pRenderEffect);
		}
	}
}