#ifndef EFE_PARAMETER_WRITER_H
#define EFE_PARAMETER_WRITER_H

#include "math/MathTypes.h"

#include "graphics/GpuProgramParameterManager.h"

namespace efe
{
	class iGpuProgramParameter;
	class cGpuProgramVectorParameter;
	class cGpuProgramMatrixParameter;
	class cGpuProgramMatrixArrayParameter;
	class cGpuProgramParameterManager;

	class iParameterWriter
	{
	public:
		iParameterWriter(){}
		virtual ~iParameterWriter(){}

		virtual iGpuProgramParameter *GetProgramParamter()=0;

		virtual void WriteParameter(cGpuProgramParameterManager *a_pManager)=0;

		virtual void InitializeParameter()=0;
	};

	typedef std::vector<iParameterWriter*> tParameterWriterVec;

	class cVectorParameterWriter : public iParameterWriter
	{
	public:
		cVectorParameterWriter() {m_vValue = cVector4f(0);}
		~cVectorParameterWriter(){}

		void SetProgramParameter(cGpuProgramVectorParameter *a_pParameter) {m_pParameter = a_pParameter;}
		void SetValue(const cVector4f &a_vValue) {m_vValue = a_vValue;}
		cVector4f GetValue() {return m_vValue;}

		iGpuProgramParameter *GetProgramParamter() {return m_pParameter;}

		void WriteParameter(cGpuProgramParameterManager *a_pManager) {a_pManager->SetVectorParameter(m_pParameter, m_vValue);}
		void InitializeParameter() {m_pParameter->InitializeParameterData(&m_vValue);}

	private:
		cGpuProgramVectorParameter *m_pParameter;
		cVector4f m_vValue;
	};

	class cMatrixParameterWriter : public iParameterWriter
	{
	public:
		cMatrixParameterWriter() {m_mtxValue = cMatrixf::Identity;}
		~cMatrixParameterWriter(){}

		void SetProgramParameter(cGpuProgramMatrixParameter *a_pParameter) {m_pParameter = a_pParameter;}
		void SetValue(const cMatrixf &a_mtxValue) {m_mtxValue = a_mtxValue;}
		cMatrixf GetValue() {return m_mtxValue;}

		iGpuProgramParameter *GetProgramParamter() {return m_pParameter;}

		void WriteParameter(cGpuProgramParameterManager *a_pManager) {a_pManager->SetMatrixParameter(m_pParameter, m_mtxValue);}
		void InitializeParameter() {m_pParameter->InitializeParameterData(&m_mtxValue);}

	private:
		cGpuProgramMatrixParameter *m_pParameter;
		cMatrixf m_mtxValue;
	};

	class cMatrixArrayParameterWriter : public iParameterWriter
	{
	public:
		cMatrixArrayParameterWriter()
		{
			m_pValue = NULL;
			m_lMatrixCount = 0;
		}
		~cMatrixArrayParameterWriter(){}

		void SetProgramParameter(cGpuProgramMatrixArrayParameter *a_pParameter) {m_pParameter = a_pParameter;}
		void SetValue(cMatrixf *a_pValue) {m_pValue = a_pValue;}
		void SetCount(int a_lCount) {m_lMatrixCount = a_lCount;}

		iGpuProgramParameter *GetProgramParamter() {return m_pParameter;}

		void WriteParameter(cGpuProgramParameterManager *a_pManager) {/*a_pManager->SetMatrixParameter()*/}
		void InitializeParameter() {m_pParameter->InitializeParameterData(m_pValue);}

	private:
		cGpuProgramMatrixArrayParameter *m_pParameter;
		cMatrixf *m_pValue;
		int m_lMatrixCount;
	};
};
#endif