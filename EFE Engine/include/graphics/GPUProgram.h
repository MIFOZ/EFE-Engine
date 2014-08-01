#ifndef EFE_GPU_PROGRAM_H
#define EFE_GPU_PROGRAM_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "resources/ResourceBase.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/GpuProgramParameter.h"

namespace efe
{
	class iTexture;

	enum eGpuProgramType
	{
		eGpuProgramType_Vertex,
		eGpuProgramType_Geometry,
		eGpuProgramType_Pixel,
		eGpuProgramType_LastEnum
	};

	enum eGpuProgramMatrix
	{
		eGpuProgramMatrix_View,
		eGpuProgramMatrix_Projection,
		eGpuProgramMatrix_Texture,
		eGpuProgramMatrix_ViewProjection,
		eGpuProgramMatrix_LastEnum
	};

	enum eGpuProgramMatrixOp
	{
		eGpuProgramMatrixOp_Identity,
		eGpuProgramMatrixOp_Inverse,
		eGpuProgramMatrixOp_Transpose,
		eGpuProgramMatrixOp_InverseTranspose,
		eGpuProgramMatrixOp_LastEnum
	};

	struct cGpuProgramDefine
	{
		tString m_sName;
		tString m_sDefinition;
	};

	typedef std::vector<cGpuProgramDefine> tGpuProgramDefineVec;
	typedef tGpuProgramDefineVec::iterator tGpuProgramDefineVecIt;

	class iGpuProgram;

	typedef std::map<tString, iGpuProgram *> tGpuProgramsMap;
	typedef tGpuProgramsMap::iterator tGpuProgramsMapIt;

	typedef std::vector<iGpuProgram*> tGpuProgramVec;
	typedef tGpuProgramVec::iterator tGpuProgramVecIt;

	class iGpuProgram : public iResourceBase
	{
		friend class cMaterialRenderEffect;

	public:
		iGpuProgram(tString a_sName, eGpuProgramType a_Type) : iResourceBase(a_sName, 0)
		{
			m_ProgramType = a_Type;
		}
		virtual ~iGpuProgram(){}

		static void SetLogDebugfInformation(bool a_bX){m_bDebugInfo = a_bX;}

		virtual bool CreateFromFile(const tString &a_sFile, const tString &a_sEntry, tGpuProgramDefineVec &a_vDefines = tGpuProgramDefineVec())=0;

		virtual void Bind()=0;

		virtual void UnBind()=0;

		//virtual void UpdateConstants()=0;

		virtual bool SetRaw(const tString &a_sName, const void *a_pData, const int a_lSize)=0;

		//virtual cSampler *GetSampler(const tString &a_sName, eGpuProgramSamplerType a_Type)=0;

		eGpuProgramType GetType(){return m_ProgramType;}
	protected:
		eGpuProgramType m_ProgramType;

		static bool m_bDebugInfo;

	private:
		// Only for render effect
		virtual tGpuProgramParameterVec GetConstantBufferParameterVec()=0;
	};
};

#endif