#ifndef EFE_GPUPARAMETER_CONTAINER_H
#define EFE_GPUPARAMETER_CONTAINER_H

#include "graphics/GpuProgramParameter.h"
#include "scene/ParameterWriter.h"

namespace efe
{
	class cGpuProgramParameterManager;

	class cGpuProgramParameterContainer
	{
	public:
		cGpuProgramParameterContainer(cGpuProgramParameterManager *a_pParameterManager);
		~cGpuProgramParameterContainer();

		void SetManager(cGpuProgramParameterManager *a_pManager) {m_pParameterManager = a_pManager;}

		void AddParameter(iParameterWriter *a_pParameter);

		iParameterWriter *GetParameterWriter(const tString &a_sName);
		cVectorParameterWriter *GetVectorParameter(const tString &a_sName);
		cMatrixParameterWriter *GetMatrixParameter(const tString &a_sName);
		cMatrixArrayParameterWriter *GetMatrixArrayParameter(const tString &a_sName);

		cVectorParameterWriter *SetVectorParameter(const tString &a_sName, const cVector4f &a_vValue);
		cMatrixParameterWriter *SetMatrixParameter(const tString &a_sName, const cMatrixf &a_mtxValue);
		cMatrixArrayParameterWriter *SetMatrixArrayParameter(const tString &a_sName, cMatrixf *a_pValues, int a_lCount);

		void SetParameters(cGpuProgramParameterManager *a_pParameterManager);
		void InitParameters();

	private:
		tParameterWriterVec m_vParameters;

		cGpuProgramParameterManager *m_pParameterManager;
	};
};
#endif 

