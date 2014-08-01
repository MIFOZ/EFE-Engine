#ifndef EFE_DX11PIPELINESYSTEM_H
#define EFE_DX11PIPELINESYSTEM_H

#include <d3d11.h>

#include "graphics/GPUProgram.h"
#include "impl/RenderPipelineSystem/RenderStages/Programmable/DX11ProgramStage.h"
#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11InputAssemblerStage.h"
#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11RasterizerStage.h"
#include "impl/RenderPipelineSystem/RenderStages/Fixed Stages/DX11OutputMergerStage.h"
#include "graphics/GpuProgramBuffer.h"

namespace efe
{
	class cGpuProgramParameterManager;
	class iGpuProgramParameter;
	class cMaterialRenderEffect;
	class cSubMesh;
	class iLowLevelGraphics;
	class iVertexFormat;

	class cDX11PipelineManager
	{
		friend class cLowLevelGraphicsDX11;
		friend class cDX11VertexBuffer;

	public:
		cDX11PipelineManager(iLowLevelGraphics *a_pLowLevelGraphics, ID3D11DeviceContext *a_pDeviceContext);
		~cDX11PipelineManager();

		void SetDeviceContext(ID3D11DeviceContext *a_pContext);

		void BindConstatntBufferParameter(eGpuProgramType a_ProgramType, iGpuProgramParameter *a_pParameter, unsigned int a_lSlot, cGpuProgramParameterManager *a_pParameterManager);
		void BindShaderResourceParameter(eGpuProgramType a_ProgramType, iGpuProgramParameter *a_pParameter, unsigned int a_lSlot, cGpuProgramParameterManager *a_pParameterManager);
		void BindUnorderedAccessParameter(eGpuProgramType a_ProgramType, iGpuProgramParameter *a_pParameter, unsigned int a_lSlot, cGpuProgramParameterManager *a_pParameterManager);
		void BindSamplerStateParameter(eGpuProgramType a_ProgramType, iGpuProgramParameter *a_pParameter, unsigned int a_lSlot, cGpuProgramParameterManager *a_pParameterManager);

		void BindProgram(eGpuProgramType a_Type, iGpuProgram *a_pProg, cGpuProgramParameterManager *a_pParameterManager);

		void ApplyInputResources();
		void ClearInputResources();

		void ApplyPipelineResources();
		void ClearPipelineResources();

		void ApplyRenderTargets();
		void ClearRenderTargets();

		void ClearPipelineState();
		void ClearPipelineSRVs();

		void Draw(cMaterialRenderEffect *a_pEffect, cSubMesh *a_pMesh, cGpuProgramParameterManager *a_pParameterManager);
		void Draw(cMaterialRenderEffect *a_pEffect, iGpuProgramBuffer *a_pVertexBuffer, iGpuProgramBuffer *a_pIndexBuffer, 
			iVertexFormat *a_pInputLayout,D3D11_PRIMITIVE_TOPOLOGY a_PrimTopology, unsigned int a_lVertexStride, unsigned int a_lVertexNum,
			cGpuProgramParameterManager *a_pParameterManager);

		void DrawIndexed(UINT a_lIndexCount, UINT a_lStartIndex, int a_lVertexOffset);

		void ClearBuffers(const cColor &a_Color, float a_fDepth, unsigned int a_lStencilRef, bool a_bClearColor, bool a_bClearDepth, bool a_bClearStencil);

		D3D11_MAPPED_SUBRESOURCE MapResource(iGpuProgramBuffer *a_pBuffer, UINT a_lSubresource, D3D11_MAP a_Actions, UINT a_lFlags);
		void UnmapResource(iGpuProgramBuffer *a_pBuffer, UINT a_lSubresource);

	private:
		ID3D11DeviceContext *m_pContext;

		cDX11ProgramStage m_VertexProgramStage;
		cDX11ProgramStage m_GeomProgramStage;
		cDX11ProgramStage m_PixelProgramStage;

		cDX11ProgramStage *m_vProgramStages[eGpuProgramType_LastEnum];

		cDX11InputAssemblerStage m_InputAssemblerStage;
		cDX11RasterizerStage m_RasterizerStage;
		cDX11OutputMergerStage m_OutputMergerStage;

		iLowLevelGraphics *m_pLowLevelGraphics;
	};
};
#endif