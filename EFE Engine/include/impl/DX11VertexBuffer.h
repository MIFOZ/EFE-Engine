#ifndef EFE_VERTEXBUFFER_DX_H
#define EFE_VERTEXBUFFER_DX_H

#include "graphics/VertexBuffer.h"
#include <D3D11.h>

namespace efe
{
	class iGpuProgramBuffer;
	class cDX11PipelineManager;

	class cDX11VertexBuffer : public iVertexBuffer
	{
	public:
		cDX11VertexBuffer(iLowLevelGraphics *a_pLowLevelGraphics, tVertexFlag a_Flags,
			eVertexBufferDrawType a_DrawType, eVertexBufferUsageType a_UsageType,
			int a_lReserveVtxSize, int a_lReserveIdxSize, cDX11PipelineManager *a_pPipelineManager);
		~cDX11VertexBuffer();

		void AddVertex(tVertexFlag a_Type, const cVector3f &a_vVtx);
		void AddColor(tVertexFlag a_Type, const cColor &a_Color);
		void AddIndex(unsigned int a_lIndex);

		void AddTriangle(unsigned int a_lA, unsigned int a_lB, unsigned int a_lC);
		void AddQuad(unsigned int a_lA, unsigned int a_lB, unsigned int a_lC, unsigned int a_lD);

		bool Compile(tVertexCompileFlag a_Flags);
		void UpdateData(tVertexFlag a_Types, bool a_lIndices);

		void CreateSkyBox();

		//void CreateShadowDouble(bool a_bUpdateData);

		void Transform(const cMatrixf &a_mtxTransform);

		void Draw(cMaterialRenderEffect *a_pEffect, eVertexBufferDrawType a_DrawType = eVertexBufferDrawType_LastEnum);
		//void DrawIndices(unsigned int *a_pIndices, int s_lCount,
				//eVertexBufferDrawType a_DrawType = eVertexBufferDrawType_LastEnum);

		void Bind();
		//void UnBind();

		//iVertexBuffer *CreateCopy(eVertexBufferUsageType a_UsageType);

		//cBoundingVolume CreateBoundingVolume();

		int GetVertexNum();
		int GetIndexNum();

		float *GetArray(tVertexFlag a_Type);
		unsigned int *GetIndices();

		void ResizeArray(tVertexFlag a_Type, int a_lSize);
		void ResizeIndices(int a_lSize);

		//D3DXVECTOR3 GetVector3(tVertexFlag a_Type, unsigned a_lIdx);
		//D3DXVECTOR3 GetVector4(tVertexFlag a_Type, unsigned a_lIdx);
		//D3DXCOLOR GetColor(tVertexFlag a_Type, unsigned a_lIdx);
		//unsigned int GetIndex(tVertexFlag a_Type, unsigned a_lIdx);

		unsigned int GetVertexSize();

	private:
		cDX11PipelineManager *m_pPipelineManager;

		iGpuProgramBuffer *m_pVertexBuffer;
		iGpuProgramBuffer *m_pIndexBuffer;

		void SetVertexState(tVertexFlag a_Flags);

		tFloatVec m_vVertexArray[klNumOfVertexFlags];
		tUIntVec m_vIndexArray;

		int m_lVtxNum;

		int m_lVtxSize;

		bool m_bTangents;

		bool m_bHasShadowDouble;

		bool m_bCompiled;

		D3D11_PRIMITIVE_TOPOLOGY GetDX11DrawTypeEnum(eVertexBufferDrawType a_Type);
	};
};

#endif