#include "graphics/GpuParameterContainer.h"

#include "graphics/GpuProgramParameterManager.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGpuProgramParameterContainer::cGpuProgramParameterContainer(cGpuProgramParameterManager *a_pParameterManager)
	{
		m_pParameterManager = a_pParameterManager;
	}

	//--------------------------------------------------------------

	cGpuProgramParameterContainer::~cGpuProgramParameterContainer()
	{
		for (iParameterWriter *pParam : m_vParameters)
			delete pParam;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cGpuProgramParameterContainer::AddParameter(iParameterWriter *a_pParameter)
	{
		if (a_pParameter)
		{
			bool bFound = false;

			for (unsigned int i = 0; i < m_vParameters.size(); i++)
			{
				if (a_pParameter->GetProgramParamter()->GetName() == m_vParameters[i]->GetProgramParamter()->GetName())
				{
					bFound = true;
					break;
				}
			}

			if (!bFound)
				m_vParameters.push_back(a_pParameter);
		}
	}

	//--------------------------------------------------------------

	iParameterWriter *cGpuProgramParameterContainer::GetParameterWriter(const tString &a_sName)
	{
		iParameterWriter *pRet = NULL;

		for (auto pParameterWriter : m_vParameters)
		{
			iGpuProgramParameter *pParam = pParameterWriter->GetProgramParamter();
			if (pParam != NULL)
			{
				if (pParam->GetName() == a_sName)
				{
					pRet = pParameterWriter;
					break;
				}
			}
		}
		return pRet;
	}

	//--------------------------------------------------------------

	cVectorParameterWriter *cGpuProgramParameterContainer::GetVectorParameter(const tString &a_sName)
	{
		iParameterWriter *pWriter = NULL;
		cVectorParameterWriter *pVectorWriter = NULL;

		pWriter = GetParameterWriter(a_sName);

		if (pWriter != NULL)
		{
			iGpuProgramParameter *pParameter = pWriter->GetProgramParamter();

			if (pParameter != NULL)
			{
				if (pParameter->GetType() == eGpuProgramParameterType_Vector)
					pVectorWriter = static_cast<cVectorParameterWriter*>(pWriter);
			}
		}

		return pVectorWriter;
	}

	//--------------------------------------------------------------

	cMatrixParameterWriter *cGpuProgramParameterContainer::GetMatrixParameter(const tString &a_sName)
	{
		iParameterWriter *pWriter = NULL;
		cMatrixParameterWriter *pMatrixWriter = NULL;

		pWriter = GetParameterWriter(a_sName);

		if (pWriter != NULL)
		{
			iGpuProgramParameter *pParameter = pWriter->GetProgramParamter();

			if (pParameter != NULL)
			{
				if (pParameter->GetType() == eGpuProgramParameterType_Matrix)
					pMatrixWriter = static_cast<cMatrixParameterWriter*>(pWriter);
			}
		}

		return pMatrixWriter;
	}

	//--------------------------------------------------------------

	cMatrixArrayParameterWriter *cGpuProgramParameterContainer::GetMatrixArrayParameter(const tString &a_sName)
	{
		iParameterWriter *pWriter = NULL;
		cMatrixArrayParameterWriter *pMatrixArrayWriter = NULL;

		pWriter = GetParameterWriter(a_sName);

		if (pWriter != NULL)
		{
			iGpuProgramParameter *pParameter = pWriter->GetProgramParamter();

			if (pParameter != NULL)
			{
				if (pParameter->GetType() == eGpuProgramParameterType_MatrixArray)
					pMatrixArrayWriter = static_cast<cMatrixArrayParameterWriter*>(pWriter);
			}
		}

		return pMatrixArrayWriter;
	}

	//--------------------------------------------------------------

	cVectorParameterWriter *cGpuProgramParameterContainer::SetVectorParameter(const tString &a_sName, const cVector4f &a_vValue)
	{
		cVectorParameterWriter *pWriter = GetVectorParameter(a_sName);

		if (pWriter == NULL)
		{
			pWriter = efeNew(cVectorParameterWriter, ());
			pWriter->SetProgramParameter(m_pParameterManager->GetVectorParameter(a_sName));
			AddParameter(pWriter);
		}

		pWriter->SetValue(a_vValue);

		return pWriter;
	}

	//--------------------------------------------------------------

	cMatrixParameterWriter *cGpuProgramParameterContainer::SetMatrixParameter(const tString &a_sName, const cMatrixf &a_mtxValue)
	{
		cMatrixParameterWriter *pWriter = GetMatrixParameter(a_sName);

		if (pWriter == NULL)
		{
			pWriter = efeNew(cMatrixParameterWriter, ());
			pWriter->SetProgramParameter(m_pParameterManager->GetMatrixParameter(a_sName));
			AddParameter(pWriter);
		}

		pWriter->SetValue(a_mtxValue);

		return pWriter;
	}

	//--------------------------------------------------------------

	cMatrixArrayParameterWriter *cGpuProgramParameterContainer::SetMatrixArrayParameter(const tString &a_sName, cMatrixf *a_pValues, int a_lCount)
	{
		cMatrixArrayParameterWriter *pWriter = GetMatrixArrayParameter(a_sName);

		if (pWriter == NULL)
		{
			pWriter = efeNew(cMatrixArrayParameterWriter, ());
			pWriter->SetProgramParameter(m_pParameterManager->GetMatrixArrayParameter(a_sName, a_lCount));
			AddParameter(pWriter);
		}

		pWriter->SetValue(a_pValues);

		return pWriter;
	}

	//--------------------------------------------------------------

	void cGpuProgramParameterContainer::SetParameters(cGpuProgramParameterManager *a_pParameterManager)
	{
		for (auto pWriter : m_vParameters)
			pWriter->WriteParameter(a_pParameterManager);
	}

	//--------------------------------------------------------------

	void cGpuProgramParameterContainer::InitParameters()
	{
		for (auto pWriter : m_vParameters)
			pWriter->InitializeParameter();
	}
}