#ifndef EFE_GPU_PROGRAM_PARAMETER_MANAGER_H
#define EFE_GPU_PROGRAM_PARAMETER_MANAGER_H

#include "graphics/GpuProgramParameter.h"

namespace efe
{
	class iGpuProgramBuffer;
	class iTexture;

	class cGpuProgramParameterManager
	{
	public:
		cGpuProgramParameterManager();

		void SetTextureParameter(const tString &a_sName, iTexture *a_pVal);
		void SetSamplerStateParameter(const tString &a_sName, int a_lSamplerPtr);

		void SetVectorParameter(cGpuProgramVectorParameter *a_pParameter, const cVector4f &a_vVal);
		void SetMatrixParameter(cGpuProgramMatrixParameter *a_pParameter, const cMatrixf &a_mtxVal);
		void SetSamplerStateParameter(cGpuProgramSamplerStateParameter *a_pParameter, int a_lSamplerPtr);
		void SetTextureParameter(cGpuProgramTextureParameter *a_pParameter, iTexture *a_pVal);

		cGpuProgramVectorParameter *GetVectorParameter(const tString &a_sName);
		cGpuProgramMatrixParameter *GetMatrixParameter(const tString &a_sName);
		cGpuProgramMatrixArrayParameter *GetMatrixArrayParameter(const tString &a_sName, int a_lMtxCount);
		cGpuProgramTextureParameter *GetResourceParameter(const tString &a_sName);
		cGpuProgramConstantBufferParameter *GetConstantBufferParameter(const tString &a_sName);
		cGpuProgramSamplerStateParameter *GetSamplerStateParameter(const tString &a_sName);

		cVector4f GetVectorParameterValue(iGpuProgramParameter *a_pParam);
		cMatrixf GetMatrixParameterValue(iGpuProgramParameter *a_pParam);
		cMatrixf *GetMatrixArrayParameterValue(iGpuProgramParameter *a_pParam);
		iGpuProgramBuffer *GetConstantBufferParameterBuffer(iGpuProgramParameter *a_pParam);

		void SetWorldMatrixParameter(const cMatrixf &a_mtxWorld);
		void SetViewMatrixParameter(const cMatrixf &a_mtxView);
		void SetProjMatrixParameter(const cMatrixf &a_mtxProj);

	private:
		iGpuProgramParameter *GetParameter(const tString &a_sName);

		tGpuProgramParameterMap m_mapParameters;

		cGpuProgramMatrixParameter *m_pWorldMatrix;
		cGpuProgramMatrixParameter *m_pViewMatrix;
		cGpuProgramMatrixParameter *m_pProjMatrix;
		cGpuProgramMatrixParameter *m_pWorldViewMatrix;
		cGpuProgramMatrixParameter *m_pViewProjMatrix;
		cGpuProgramMatrixParameter *m_pWorldViewProjMatrix;
	};
};

#endif