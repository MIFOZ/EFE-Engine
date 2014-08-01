#include "graphics/GpuProgramParameterManager.h"

#include "math/Math.h"
#include "graphics/Texture.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGpuProgramParameterManager::cGpuProgramParameterManager()
	{
		m_pWorldMatrix = GetMatrixParameter("World");
		m_pViewMatrix = GetMatrixParameter("View");
		m_pProjMatrix = GetMatrixParameter("Proj");
		m_pWorldViewMatrix = GetMatrixParameter("WorldView");
		m_pViewProjMatrix = GetMatrixParameter("ViewProj");
		m_pWorldViewProjMatrix = GetMatrixParameter("WorldViewProj");
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cGpuProgramParameterManager::SetTextureParameter(const tString &a_sName, iTexture *a_pVal)
	{
		iGpuProgramParameter *pParam = m_mapParameters[a_sName];

		if (pParam == NULL)
		{
			pParam = efeNew(cGpuProgramTextureParameter, (a_sName));
			m_mapParameters[a_sName] = pParam;

			pParam->InitializeParameterData(a_pVal);
		}
		else
		{
			if (pParam->GetType() == eGpuProgramParameterType_Texture)
				pParam->SetParameterData(a_pVal);
		}
	}

	//--------------------------------------------------------------

	void cGpuProgramParameterManager::SetSamplerStateParameter(const tString &a_sName, int a_lSamplerPtr)
	{
		iGpuProgramParameter *pParam = m_mapParameters[a_sName];

		if (pParam == NULL)
		{
			pParam = efeNew(cGpuProgramSamplerStateParameter, (a_sName));
			m_mapParameters[a_sName] = pParam;

			pParam->InitializeParameterData((void*)a_lSamplerPtr);
		}
		else
		{
			if (pParam->GetType() == eGpuProgramParameterType_SamplerState)
				pParam->SetParameterData((void*)a_lSamplerPtr);
		}
	}

	//--------------------------------------------------------------

	void cGpuProgramParameterManager::SetVectorParameter(cGpuProgramVectorParameter *a_pParameter, const cVector4f &a_vVal)
	{
		if (a_pParameter->GetType() == eGpuProgramParameterType_Vector)
			a_pParameter->SetParameterData((void*)&a_vVal);
	}

	//--------------------------------------------------------------

	void cGpuProgramParameterManager::SetMatrixParameter(cGpuProgramMatrixParameter *a_pParameter, const cMatrixf &a_mtxVal)
	{
		if (a_pParameter->GetType() == eGpuProgramParameterType_Matrix)
			a_pParameter->SetParameterData((void*)&a_mtxVal);
	}

	//--------------------------------------------------------------

	void cGpuProgramParameterManager::SetTextureParameter(cGpuProgramTextureParameter *a_pParameter, iTexture *a_pVal)
	{
		if (a_pParameter->GetType() == eGpuProgramParameterType_Texture)
			a_pParameter->SetParameterData(a_pVal);
	}

	//--------------------------------------------------------------

	cGpuProgramVectorParameter *cGpuProgramParameterManager::GetVectorParameter(const tString &a_sName)
	{
		iGpuProgramParameter *pParam = GetParameter(a_sName);

		if (pParam == NULL)
		{
			pParam = efeNew(cGpuProgramVectorParameter, (a_sName));
			m_mapParameters[a_sName] = pParam;
		}

		return static_cast<cGpuProgramVectorParameter*>(pParam);
	}

	//--------------------------------------------------------------

	cGpuProgramMatrixParameter *cGpuProgramParameterManager::GetMatrixParameter(const tString &a_sName)
	{
		cGpuProgramMatrixParameter *pParam = static_cast<cGpuProgramMatrixParameter*>(GetParameter(a_sName));

		if (pParam == NULL)
		{
			pParam = efeNew(cGpuProgramMatrixParameter, (a_sName));
			m_mapParameters[a_sName] = pParam;
		}

		return pParam;
	}

	cGpuProgramMatrixArrayParameter *cGpuProgramParameterManager::GetMatrixArrayParameter(const tString &a_sName, int a_lMtxCount)
	{
		cGpuProgramMatrixArrayParameter *pParam = static_cast<cGpuProgramMatrixArrayParameter*>(GetParameter(a_sName));

		if (pParam == NULL)
		{
			pParam = efeNew(cGpuProgramMatrixArrayParameter, (a_sName, a_lMtxCount));
			m_mapParameters[a_sName] = pParam;
		}

		return pParam;
	}

	cGpuProgramTextureParameter *cGpuProgramParameterManager::GetResourceParameter(const tString &a_sName)
	{
		cGpuProgramTextureParameter *pParam = static_cast<cGpuProgramTextureParameter*>(GetParameter(a_sName));

		if (pParam == NULL)
		{
			pParam = efeNew(cGpuProgramTextureParameter, (a_sName));
			m_mapParameters[a_sName] = pParam;
		}

		return pParam;
	}

	cGpuProgramConstantBufferParameter *cGpuProgramParameterManager::GetConstantBufferParameter(const tString &a_sName)
	{
		cGpuProgramConstantBufferParameter *pParam = static_cast<cGpuProgramConstantBufferParameter*>(GetParameter(a_sName));

		if (pParam == NULL)
		{
			pParam = efeNew(cGpuProgramConstantBufferParameter, (a_sName));
			m_mapParameters[a_sName] = pParam;
		}

		return pParam;
	}

	//--------------------------------------------------------------

	cGpuProgramSamplerStateParameter *cGpuProgramParameterManager::GetSamplerStateParameter(const tString &a_sName)
	{
		cGpuProgramSamplerStateParameter *pParam = static_cast<cGpuProgramSamplerStateParameter*>(GetParameter(a_sName));

		if (pParam == NULL)
		{
			pParam = efeNew(cGpuProgramSamplerStateParameter, (a_sName));
			m_mapParameters[a_sName] = pParam;
		}

		return pParam;
	}

	//--------------------------------------------------------------

	cVector4f cGpuProgramParameterManager::GetVectorParameterValue(iGpuProgramParameter *a_pParam)
	{
		cVector4f vRet;

		if (a_pParam->GetType() == eGpuProgramParameterType_Vector)
			vRet = static_cast<cGpuProgramVectorParameter*>(a_pParam)->GetVector();

		return vRet;
	}

	//--------------------------------------------------------------

	cMatrixf cGpuProgramParameterManager::GetMatrixParameterValue(iGpuProgramParameter *a_pParam)
	{
		cMatrixf mtxRet = cMatrixf::Identity;

		if (a_pParam->GetType() == eGpuProgramParameterType_Matrix)
			mtxRet = static_cast<cGpuProgramMatrixParameter*>(a_pParam)->GetMatrix();

		return mtxRet;
	}

	//--------------------------------------------------------------

	cMatrixf *cGpuProgramParameterManager::GetMatrixArrayParameterValue(iGpuProgramParameter *a_pParam)
	{
		cMatrixf *pRet = NULL;

		if (a_pParam->GetType() == eGpuProgramParameterType_MatrixArray)
			pRet = static_cast<cGpuProgramMatrixArrayParameter*>(a_pParam)->GetMatrices();

		return pRet;
	}

	//--------------------------------------------------------------

	iGpuProgramBuffer *cGpuProgramParameterManager::GetConstantBufferParameterBuffer(iGpuProgramParameter *a_pParam)
	{
		iGpuProgramBuffer *pBuffer = NULL;

		if (a_pParam->GetType() == eGpuProgramParameterType_ConstantBuffer)
		{
			pBuffer = static_cast<cGpuProgramConstantBufferParameter*>(a_pParam)->GetBuffer();
		}
		return pBuffer;
	}

	void cGpuProgramParameterManager::SetWorldMatrixParameter(const cMatrixf &a_mtxWorld)
	{
		SetMatrixParameter(m_pWorldMatrix, a_mtxWorld);

		cMatrixf mtxWorld = GetMatrixParameterValue(m_pWorldMatrix);
		cMatrixf mtxView = GetMatrixParameterValue(m_pViewMatrix);
		cMatrixf mtxProj = GetMatrixParameterValue(m_pProjMatrix);

		cMatrixf mtxWorldView = cMath::MatrixMul(mtxView, mtxWorld);
		cMatrixf mtxViewProj = cMath::MatrixMul(mtxProj, mtxView);
		cMatrixf mtxWorldViewProj = cMath::MatrixMul(mtxProj, cMath::MatrixMul(mtxView, mtxWorld));

		SetMatrixParameter(m_pWorldViewMatrix, mtxWorldView);
		SetMatrixParameter(m_pViewProjMatrix, mtxViewProj);
		SetMatrixParameter(m_pWorldViewProjMatrix, mtxWorldViewProj);
	}

	void cGpuProgramParameterManager::SetViewMatrixParameter(const cMatrixf &a_mtxView)
	{
		SetMatrixParameter(m_pViewMatrix, a_mtxView);

		cMatrixf mtxWorld = GetMatrixParameterValue(m_pWorldMatrix);
		cMatrixf mtxView = GetMatrixParameterValue(m_pViewMatrix);
		cMatrixf mtxProj = GetMatrixParameterValue(m_pProjMatrix);

		cMatrixf mtxWorldView = cMath::MatrixMul(mtxView, mtxWorld);
		cMatrixf mtxViewProj = cMath::MatrixMul(mtxProj, mtxView);
		cMatrixf mtxWorldViewProj = cMath::MatrixMul(mtxProj, cMath::MatrixMul(mtxView, mtxWorld));

		SetMatrixParameter(m_pWorldViewMatrix, mtxWorldView);
		SetMatrixParameter(m_pViewProjMatrix, mtxViewProj);
		SetMatrixParameter(m_pWorldViewProjMatrix, mtxWorldViewProj);
	}
	void cGpuProgramParameterManager::SetProjMatrixParameter(const cMatrixf &a_mtxProj)
	{
		SetMatrixParameter(m_pProjMatrix, a_mtxProj);

		cMatrixf mtxWorld = GetMatrixParameterValue(m_pWorldMatrix);
		cMatrixf mtxView = GetMatrixParameterValue(m_pViewMatrix);
		cMatrixf mtxProj = GetMatrixParameterValue(m_pProjMatrix);

		cMatrixf mtxWorldView = cMath::MatrixMul(mtxView, mtxWorld);
		cMatrixf mtxViewProj = cMath::MatrixMul(mtxProj, mtxView);
		cMatrixf mtxWorldViewProj = cMath::MatrixMul(mtxProj, cMath::MatrixMul(mtxView, mtxWorld));

		SetMatrixParameter(m_pWorldViewMatrix, mtxWorldView);
		SetMatrixParameter(m_pViewProjMatrix, mtxViewProj);
		SetMatrixParameter(m_pWorldViewProjMatrix, mtxWorldViewProj);
	}

	iGpuProgramParameter *cGpuProgramParameterManager::GetParameter(const tString &a_sName)
	{
		iGpuProgramParameter *pParam = m_mapParameters[a_sName];

		return pParam;
	}
}