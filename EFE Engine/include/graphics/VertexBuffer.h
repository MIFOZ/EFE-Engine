#ifndef EFE_VERTEXBUFFER_H
#define EFE_VERTEXBUFFER_H

#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "math/BoundingVolume.h"

namespace efe
{
	class cBoundingVolume;

	enum eVertexBufferDrawType
	{
		eVertexBufferDrawType_LineList,
		eVertexBufferDrawType_LineStrip,
		eVertexBufferDrawType_TriangleList,
		eVertexBufferDrawType_TriangleStrip,
		eVertexBufferDrawType_LastEnum
	};

	enum eVertexBufferUsageType
	{
		eVertexBufferUsageType_Static,
		eVertexBufferUsageType_Dynamic,
		eVertexBufferUsageType_Stream,
		eVertexBufferUsageType_LastEnum
	};

	enum eAttributeType
	{
		eAttributeType_Generic,
		eAttributeType_Vertex,
		eAttributeType_Color,
		eAttributeType_TexCoord,
		eAttributeType_Normal,
		eAttributeType_Tangent,
		eAttributeType_Binormal
	};

	class cMaterialRenderEffect;

	typedef tFlag tVertexFlag;

#define eVertexFlag_Normal		(0x00000001)
#define eVertexFlag_Position	(0x00000002)
#define eVertexFlag_Color0		(0x00000004)
#define eVertexFlag_Color1		(0x00000008)
#define eVertexFlag_Texture0	(0x00000010)
#define eVertexFlag_Tangent		(0x00000020)
#define eVertexFlag_Texture2	(0x00000040)
#define eVertexFlag_Texture3	(0x00000080)
#define eVertexFlag_Texture4	(0x00000100)

#define klNumOfVertexFlags (9)

	const tVertexFlag kvVertexFlags[] = {eVertexFlag_Normal,eVertexFlag_Position,eVertexFlag_Color0,
		eVertexFlag_Color1,eVertexFlag_Texture0,eVertexFlag_Tangent,eVertexFlag_Texture2,
		eVertexFlag_Texture3,eVertexFlag_Texture4};

	const int kvVertexElements[] = {3, 4, 4, 4, 3, 4, 3, 3 ,3};

	typedef tFlag tVertexCompileFlag;

#define eVertexCompileFlag_CreateTangents	(0x00000001)

	class iLowLevelGraphics;

	struct iVertexFormat
	{
		virtual ~iVertexFormat(){}

		unsigned int m_lVertexSize[MAX_VERTEXSTREAMS];
	};

	class iVertexBuffer
	{
	public:
		iVertexBuffer(iLowLevelGraphics *a_pLowLevelGraphics, tVertexFlag a_Flags,
			eVertexBufferDrawType a_DrawType, eVertexBufferUsageType a_UsageType,
			int a_lReserveVtxSize, int a_lReserveIdxSize) :
		m_VertexFlags(a_Flags), m_pLowLevelGraphics(a_pLowLevelGraphics),
			m_DrawType(a_DrawType), m_UsageType(a_UsageType), m_lElementNum(-1),
			m_bTangents(false){}

		virtual ~iVertexBuffer(){}

		tVertexFlag GetFlags(){return m_VertexFlags;}

		virtual void AddVertex(tVertexFlag a_Type, const cVector3f &a_vVtx)=0;
		virtual void AddColor(tVertexFlag a_Type, const cColor &a_Color)=0;
		virtual void AddIndex(unsigned int a_lIndex)=0;

		virtual void AddTriangle(unsigned int a_lA, unsigned int a_lB, unsigned int a_lC)=0;
		virtual void AddQuad(unsigned int a_lA, unsigned int a_lB, unsigned int a_lC, unsigned int a_lD)=0;

		virtual bool Compile(tVertexCompileFlag a_Flags)=0;
		virtual void UpdateData(tVertexFlag a_Types, bool a_lIndices)=0;

		virtual void CreateSkyBox()=0;

		//virtual void CreateShadowDouble(bool a_bUpdateData)=0;

		virtual void Transform(const cMatrixf &a_mtxTransform)=0;

		virtual void Draw(cMaterialRenderEffect *a_pEffect = NULL, eVertexBufferDrawType a_DrawType = eVertexBufferDrawType_LastEnum)=0;
		//virtual void DrawIndices(unsigned int *a_pIndices, int s_lCount,
		//eVertexBufferDrawType a_DrawType = eVertexBufferDrawType_LastEnum)=0;

		virtual void Bind()=0;
		//virtual void UnBind()=0;

		//virtual iVertexBuffer *CreateCopy(eVertexBufferUsageType a_UsageType)=0;

		//virtual cBoundingVolume CreateBoundingVolume()=0;

		virtual float *GetArray(tVertexFlag a_Type)=0;
		virtual unsigned int *GetIndices()=0;

		virtual int GetVertexNum()=0;
		virtual int GetIndexNum()=0;

		virtual void ResizeArray(tVertexFlag a_Type, int a_lSize)=0;
		virtual void ResizeIndices(int a_lSize)=0;

		//virtual D3DXVECTOR3 GetVector3(tVertexFlag a_Type, unsigned a_lIdx)=0;
		//virtual D3DXVECTOR3 GetVector4(tVertexFlag a_Type, unsigned a_lIdx)=0;
		//virtual D3DXCOLOR GetColor(tVertexFlag a_Type, unsigned a_lIdx)=0;
		//virtual unsigned int GetIndex(tVertexFlag a_Type, unsigned a_lIdx)=0;

		//void SetElementsNum(int a_lNum){m_lElementNum = a_lNum;}
		//int GetElementNum(){return m_lElementNum;}

		//tVertexFlag GetVertexFlag(){return m_VertexFlags;}

		virtual unsigned int GetVertexSize()=0;

		bool HasTangents(){return m_bTangents;}
		void SetTangents(bool a_bX){m_bTangents = a_bX;}
	protected:
		tVertexFlag m_VertexFlags;
		eVertexBufferDrawType m_DrawType;
		eVertexBufferUsageType m_UsageType;
		iLowLevelGraphics *m_pLowLevelGraphics;

		int m_lElementNum;

		bool m_bTangents;
	};
};

#endif