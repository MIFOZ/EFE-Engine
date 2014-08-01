#ifndef EFE_DX11_BUFFER_H
#define EFE_DX11_BUFFER_H

#include "graphics/GpuProgramBuffer.h"

#include <D3DX11.h>

#include <vector>

namespace efe
{
	class cDX11ProgramBuffer : public iGpuProgramBuffer
	{
		friend class cDX11PipelineManager;

	public:
		cDX11ProgramBuffer(eGpuProgramBufferType a_Type, ID3D11Buffer *a_pDXBuffer)
			: iGpuProgramBuffer(a_Type), m_pDXBuffer(a_pDXBuffer) {}
		virtual ~cDX11ProgramBuffer()
		{
			m_pDXBuffer->Release();
		}

		void SetDesiredDescription(D3D11_BUFFER_DESC a_Desc);

		ID3D11Buffer *GetBuffer(){return m_pDXBuffer;}

	protected:
		ID3D11Buffer *m_pDXBuffer;

		D3D11_BUFFER_DESC m_DesiredDesc;
		D3D11_BUFFER_DESC m_ActualDesc;
	};

	class cDX11ProgramBufferConfig : public iGpuProgramBufferConfig
	{
		friend class cLowLevelGraphicsDX11;

	public:
		void SetDefaults();

		void SetConstantBuffer(unsigned int a_lSize, bool a_bDynamic);
		void SetVertexBuffer(eBufferUsage a_Usage, unsigned int a_lSize, unsigned int a_lStride);
		void SetIndexBuffer(eBufferUsage a_Usage, unsigned int a_lSize, unsigned int a_lStride);

	private:
		D3D11_BUFFER_DESC m_State;
	};	

	class iGpuProgramParameter; 

	struct cConstantBufferMapping
	{
		iGpuProgramParameter *m_pParameter;
		unsigned int m_lOffset;
		unsigned int m_lSize;
		D3D_SHADER_VARIABLE_CLASS m_VarClass;
		unsigned int m_lElementsNum;
		unsigned int m_lValueIdx;
	};

	typedef std::vector<cConstantBufferMapping> tConstantBufferMappingVec;
	typedef tConstantBufferMappingVec::iterator tConstantBufferMappingVecIt;

	class cDX11PipelineManager;
	class cGpuProgramParameterManager;

	class cDX11ProgramConstantBuffer : public cDX11ProgramBuffer
	{
	public:
		cDX11ProgramConstantBuffer(ID3D11Buffer *a_pDXBuffer);

		void AddMapping(cConstantBufferMapping &a_Mapping);
		void ClearMappings();
		void EvaluateMapping(cDX11PipelineManager *a_pDXPipeline, cGpuProgramParameterManager *a_pParameterManager);
		bool ContainsMapping(unsigned int a_lIdx, const cConstantBufferMapping &a_Mapping);

		void SetAutoUpdate(bool a_bX);
		bool GetAutoUpdate();

	private:
		bool m_bAutoUpdate;
		tConstantBufferMappingVec m_vMappings;
	};
};

#endif