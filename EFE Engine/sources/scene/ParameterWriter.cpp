#include "scene/ParameterWriter.h"



namespace efe
{
	//////////////////////////////////////////////////////////////
	// VECTOR WRITER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

/* 	cVectorParameterWriter::cVectorParameterWriter()
// 	{
// 		m_vValue = 0;
// 	}

	//--------------------------------------------------------------

// 	void cVectorParameterWriter::SetProgramParameter(cGpuProgramVectorParameter *a_pParameter)
// 	{
// 		m_pParameter = a_pParameter;
// 	}

	//--------------------------------------------------------------

	void cVectorParameterWriter::SetValue(const cVector4f &a_vValue)
	{
		m_vValue = a_vValue;
	}

	//--------------------------------------------------------------

	cVector4f cVectorParameterWriter::GetValue()
	{
		return m_vValue;
	}

	//--------------------------------------------------------------

	iGpuProgramParameter *cVectorParameterWriter::GetProgramParamter()
	{
		return m_pParameter;
	}

	//--------------------------------------------------------------

	void cVectorParameterWriter::WriteParameter(cGpuProgramParameterManager *a_pManager)
	{
		a_pManager->SetVectorParameter(m_pParameter, m_vValue);
	}

	//--------------------------------------------------------------

	void cVectorParameterWriter::InitializeParameter()
	{
		m_pParameter->InitializeParameterData(&m_vValue);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// MATRIX WRITER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMatrixParameterWriter::cMatrixParameterWriter()
	{
		m_mtxValue = cMatrixf::Identity;
	}

	//--------------------------------------------------------------

	void cMatrixParameterWriter::SetProgramParameter(cGpuProgramMatrixParameter *a_pParameter)
	{
		m_pParameter = a_pParameter;
	}

	//--------------------------------------------------------------

	void cMatrixParameterWriter::SetValue(const cMatrixf &a_mtxValue)
	{
		m_mtxValue = a_mtxValue;
	}

	//--------------------------------------------------------------

	cMatrixf cMatrixParameterWriter::GetValue()
	{
		return m_mtxValue;
	}

	//--------------------------------------------------------------

	iGpuProgramParameter *cMatrixParameterWriter::GetProgramParamter()
	{
		return m_pParameter;
	}

	//--------------------------------------------------------------

	void cMatrixParameterWriter::WriteParameter(cGpuProgramParameterManager *a_pManager)
	{
		a_pManager->SetMatrixParameter(m_pParameter, m_mtxValue);
	}

	//--------------------------------------------------------------

	void cMatrixParameterWriter::InitializeParameter()
	{
		m_pParameter->InitializeParameterData(&m_mtxValue);
	}

	//--------------------------------------------------------------

	cMatrixArrayParameterWriter::cMatrixArrayParameterWriter()
	{
		m_pValue = NULL;
		m_lMatrixCount = 0;
	}

	//--------------------------------------------------------------

	void cMatrixArrayParameterWriter::SetProgramParameter(cGpuProgramMatrixArrayParameter *a_pParameter)
	{
		m_pParameter = a_pParameter;
	}

	//--------------------------------------------------------------

	void cMatrixArrayParameterWriter::SetValue(cMatrixf *a_pValue)
	{
		m_pValue = a_pValue;
	}

	//--------------------------------------------------------------

	void cMatrixArrayParameterWriter::SetCount(int a_lCount)
	{
		m_lMatrixCount = a_lCount;
	}*/
}