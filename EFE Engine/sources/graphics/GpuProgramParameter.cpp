#include "graphics/GpuProgramParameter.h"
#include "system/LowLevelSystem.h"

namespace efe
{
	void iGpuProgramParameter::InitializeParameterData(void *a_pData)
	{
		SetParameterData(a_pData);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// VECTOR PARAMETER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGpuProgramVectorParameter::cGpuProgramVectorParameter(const tString &a_sName)
		: iGpuProgramParameter(eGpuProgramParameterType_Vector, a_sName)
	{

	}

	//--------------------------------------------------------------

	void cGpuProgramVectorParameter::SetParameterData(void *a_pData)
	{
		if (memcmp(a_pData, (void*)&m_vVector, sizeof(cVector4f)) != 0)
		{
			m_vVector = *reinterpret_cast<cVector4f*>(a_pData);
		}
	}

	//--------------------------------------------------------------

	void cGpuProgramVectorParameter::SetVector(const cVector4f &a_vVal)
	{
		m_vVector = a_vVal;
	}

	//--------------------------------------------------------------

	cVector4f cGpuProgramVectorParameter::GetVector()
	{
		return m_vVector;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// MATRIX PARAMETER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cGpuProgramMatrixParameter::SetParameterData(void *a_pData)
	{
		if (memcmp(a_pData, &m_mtxValue, sizeof(cMatrixf)))
		{
			m_mtxValue = *reinterpret_cast<cMatrixf*>(a_pData);
			m_lValueCount++;
		}
	}

	//--------------------------------------------------------------

	cMatrixf cGpuProgramMatrixParameter::GetMatrix()
	{
		return m_mtxValue;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// MATRIX ARRAY PARAMETER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGpuProgramMatrixArrayParameter::cGpuProgramMatrixArrayParameter(const tString &a_sName, int a_lMtxCount)
		: iGpuProgramParameter(eGpuProgramParameterType_MatrixArray, a_sName)
	{
		if (a_lMtxCount < 1)
			a_lMtxCount = 1;

		m_lMatrixCount = a_lMtxCount;

		m_pMatrices = efeNewArray(cMatrixf, m_lMatrixCount);
	}

	//--------------------------------------------------------------

	cGpuProgramMatrixArrayParameter::~cGpuProgramMatrixArrayParameter()
	{
		efeDeleteArray(m_pMatrices);
	}

	//--------------------------------------------------------------

	void cGpuProgramMatrixArrayParameter::SetParameterData(void *a_pData)
	{
		if (memcmp(&m_pMatrices, a_pData, sizeof(cMatrixf) * m_lMatrixCount))
		{
			m_lValueCount++;
			memcpy(m_pMatrices, a_pData, sizeof(cMatrixf) * m_lMatrixCount);
		}
	}

	//--------------------------------------------------------------

	int cGpuProgramMatrixArrayParameter::GetMatrixCount()
	{
		return m_lMatrixCount;
	}

	//--------------------------------------------------------------

	cMatrixf *cGpuProgramMatrixArrayParameter::GetMatrices()
	{
		return m_pMatrices;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// TEXTURE PARAMETER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGpuProgramTextureParameter::cGpuProgramTextureParameter(const tString &a_sName)
		: iGpuProgramParameter(eGpuProgramParameterType_Texture, a_sName)
	{
		m_pData = NULL;
	}

	void cGpuProgramTextureParameter::SetParameterData(void *a_pData)
	{
		m_pData = a_pData;
	}

	//--------------------------------------------------------------

	void *cGpuProgramTextureParameter::GetTexture()
	{
		return m_pData;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// TEXTURE SAMPLER PARAMETER
	//////////////////////////////////////////////////////////////

	//---------------------------------------------------------------

	cGpuProgramSamplerStateParameter::cGpuProgramSamplerStateParameter(const tString &a_sName)
		: iGpuProgramParameter(eGpuProgramParameterType_SamplerState, a_sName)
	{
		m_lSamplerPtr = 0;
	}

	void cGpuProgramSamplerStateParameter::SetParameterData(void *a_pData)
	{
		m_lSamplerPtr = (int)a_pData;
	}

	//--------------------------------------------------------------

	int cGpuProgramSamplerStateParameter::GetSamplerPtr()
	{
		return m_lSamplerPtr;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTANT BUFFER PARAMETER
	//////////////////////////////////////////////////////////////

	//---------------------------------------------------------------

	cGpuProgramConstantBufferParameter::cGpuProgramConstantBufferParameter(const tString &a_sName)
		: iGpuProgramParameter(eGpuProgramParameterType_ConstantBuffer, a_sName)
	{
		m_pBuffer = NULL;
	}

	//---------------------------------------------------------------
	
	void cGpuProgramConstantBufferParameter::SetParameterData(void *a_pData)
	{
		m_pBuffer = (iGpuProgramBuffer *)a_pData;
	}

	//---------------------------------------------------------------

	iGpuProgramBuffer *cGpuProgramConstantBufferParameter::GetBuffer()
	{
		return m_pBuffer;
	}
}