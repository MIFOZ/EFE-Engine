#ifndef EFE_GPU_PROGRAM_PARAMETER_H
#define EFE_GPU_PROGRAM_PARAMETER_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"

namespace efe
{
	class iGpuProgramBuffer;

	enum eGpuProgramParameterType
	{
		eGpuProgramParameterType_Vector,
		eGpuProgramParameterType_Matrix,
		eGpuProgramParameterType_MatrixArray,
		eGpuProgramParameterType_Texture,
		eGpuProgramParameterType_ConstantBuffer,
		eGpuProgramParameterType_SamplerState
	};

	class iGpuProgramParameterBuffer
	{
	public:
		iGpuProgramParameterBuffer();

	protected:
		bool m_bAutoUpdate;
	};

	class iGpuProgramParameter
	{
	public:
		iGpuProgramParameter(eGpuProgramParameterType a_Type, const tString &a_sName)
			: m_Type(a_Type), m_lValueCount(0), m_sName(a_sName){}
		virtual ~iGpuProgramParameter(){}

		virtual void SetParameterData(void *a_pData) = 0;

		eGpuProgramParameterType GetType(){return m_Type;}
		tString GetName(){return m_sName;}

		void InitializeParameterData(void *a_pData);

		unsigned int GetValueIdx(){return m_lValueCount;}

	protected:
		unsigned int m_lValueCount;

	private:
		eGpuProgramParameterType m_Type;

		tString m_sName;
	};

	typedef std::vector<iGpuProgramParameter*> tGpuProgramParameterVec;
	typedef tGpuProgramParameterVec::iterator tGpuProgramParameterVecIt;

	typedef std::map<tString, iGpuProgramParameter*> tGpuProgramParameterMap;
	typedef tGpuProgramParameterMap::iterator tGpuProgramParameterMapIt;

	class cGpuProgramVectorParameter : public iGpuProgramParameter
	{
	public:
		cGpuProgramVectorParameter(const tString &a_sName);

		void SetParameterData(void *a_pData);

		void SetVector(const cVector4f &a_vVal);
		cVector4f GetVector();

	private:
		cVector4f m_vVector;
	};

	class cGpuProgramMatrixParameter : public iGpuProgramParameter
	{
	public:
		cGpuProgramMatrixParameter(const tString &a_sName)
			: iGpuProgramParameter(eGpuProgramParameterType_Matrix, a_sName){}

		void SetParameterData(void *a_pData);

		cMatrixf GetMatrix();

	private:
		cMatrixf m_mtxValue;
	};

	class cGpuProgramMatrixArrayParameter : public iGpuProgramParameter
	{
	public:
		cGpuProgramMatrixArrayParameter(const tString &a_sName, int a_lMtxCount);
		~cGpuProgramMatrixArrayParameter();

		void SetParameterData(void *a_pData);

		int GetMatrixCount();
		cMatrixf *GetMatrices();

	private:
		int m_lMatrixCount;
		cMatrixf *m_pMatrices;
	};

	class cGpuProgramTextureParameter : public iGpuProgramParameter
	{
	public:
		cGpuProgramTextureParameter(const tString &a_sName);

		void SetParameterData(void *a_pData);

		void *GetTexture();

	private:
		void *m_pData;
	};

	class cGpuProgramSamplerStateParameter : public iGpuProgramParameter
	{
	public:
		cGpuProgramSamplerStateParameter(const tString &a_sName);

		void SetParameterData(void *a_pData);

		//************************************
		// Method:    GetSamplerPtr
		// FullName:  efe::cGpuProgramSamplerStateParameter::GetSampler
		// Access:    public 
		// Returns:   int
		// Qualifier: returns a pointer to API interface casted to an int
		//************************************
		int GetSamplerPtr();

	private:
		int m_lSamplerPtr; // just an API interface pointer casted to an int
	};

	class cGpuProgramConstantBufferParameter : public iGpuProgramParameter
	{
	public:
		cGpuProgramConstantBufferParameter(const tString &a_sName);

		void SetParameterData(void *a_pData);

		iGpuProgramBuffer *GetBuffer();

	private:
		iGpuProgramBuffer *m_pBuffer;
	};
};

#endif