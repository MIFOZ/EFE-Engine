#include "graphics/MaterialRenderEffect.h"
#include "graphics/LowLevelGraphics.h"

#include "graphics/GpuProgramParameterManager.h"

#include "system/SystemTypes.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMaterialRenderEffect::cMaterialRenderEffect()
	{
		for (int i=0; i<eGpuProgramType_LastEnum; i++)
			m_vPrograms[i] = NULL;
	}

	cMaterialRenderEffect::~cMaterialRenderEffect()
	{

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cMaterialRenderEffect::SetProgram(iGpuProgram *a_pProgram, eGpuProgramType a_Type)
	{
		if (a_pProgram->GetType() != a_Type)
			Warning("Trying to bind a gpu program to the wrong slot in a render effect!\n");

		m_vPrograms[a_Type] = a_pProgram;

		UpdateConstantBufferList();
	}

	//--------------------------------------------------------------

	iGpuProgram *cMaterialRenderEffect::GetProgram(eGpuProgramType a_Type)
	{
		return m_vPrograms[a_Type];
	}

	//--------------------------------------------------------------

	void cMaterialRenderEffect::ConfigurePipeline(iLowLevelGraphics *a_pLowGfx, cGpuProgramParameterManager *a_pParamManager)
	{
		//a_pLowGfx->setfill
		//a_pLowGfx->SetBlendState(m_pBlendState);

		//a_pLowGfx->SetDepthState(m_pDepthStencilState);
		//assert(0); // TODO: add stencil ref setting
// 		if (m_pDepthStencilState)
// 			a_pLowGfx->set

		//a_pLowGfx->SetRasterizerState(m_pRasterizerState);

		for (auto pParameter : m_vUniqueConstBuffers)
		{
			iGpuProgramBuffer *pBuffer = a_pParamManager->GetConstantBufferParameterBuffer(pParameter);
			a_pLowGfx->EvaluateConstantBufferMappings(pBuffer);
		}



		a_pLowGfx->BindProgram(m_vPrograms[eGpuProgramType_Vertex], eGpuProgramType_Vertex);
		a_pLowGfx->BindProgram(m_vPrograms[eGpuProgramType_Geometry], eGpuProgramType_Geometry);
		a_pLowGfx->BindProgram(m_vPrograms[eGpuProgramType_Pixel], eGpuProgramType_Pixel);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cMaterialRenderEffect::UpdateConstantBufferList()
	{
		m_vUniqueConstBuffers.clear();

		for (int stage = 0; stage < (int)eGpuProgramType_LastEnum; stage++)
		{
			iGpuProgram *pProg = m_vPrograms[stage];

			if (pProg != NULL)
			{
				tGpuProgramParameterVec &vParameters = pProg->GetConstantBufferParameterVec();
				for (size_t i = 0; i < vParameters.size(); i++)
				{
					iGpuProgramParameter *pParam = vParameters[i];

					bool bFound = false;
					for (auto pExistingParam : m_vUniqueConstBuffers)
					{
						if (pExistingParam == pParam)
						{
							bFound = true;
							break;
						}
					}

					if (!bFound)
					{
						m_vUniqueConstBuffers.push_back(pParam);
					}
				}
			}
		}
	}

	eFillMode cMaterialRenderEffect::GetFillModeType(eMaterialFillMode a_Mode)
	{
		switch (a_Mode)
		{
		case efe::eMaterialFillMode_Solid: return eFillMode_Solid;
		case efe::eMaterialFillMode_Wireframe: return eFillMode_Wireframe;
		default: assert(0);
		}
	}
}