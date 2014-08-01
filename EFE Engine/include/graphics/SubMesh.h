#ifndef EFE_SUB_MESH_H
#define EFE_SUB_MESH_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "math/MeshTypes.h"

namespace efe
{
	class iMaterial;
	class iVertexBuffer;

	class cMesh;

	class cMaterialManager;

	class cSubMesh
	{
		friend class cMesh;
		friend class cSubMeshEntity;
	public:
		cSubMesh(const tString &a_sName, cMaterialManager *a_pMaterialManager);
		~cSubMesh();

		void SetMaterial(iMaterial *a_pMaterial);
		void SetVertexBuffer(iVertexBuffer *a_pVtxBuffer);

		//Renderable implementation
		iMaterial *GetMaterial();
		iVertexBuffer *GetVertexBuffer();

		tString GetName(){return m_sName;}

		tTriEdgeVec *GetEdgeVecPtr(){return &m_vEdges;}

		tTriangleDataVec *GetTriangleVecPtr(){return &m_vTriangles;}

		void SetDoubleSided(bool a_bX){m_bDoubleSided = a_bX;}

		const tString &GetGroup(){return m_sGroup;}
		void SetGroup(const tString &a_sGroup){m_sGroup = a_sGroup;}

		const tString &GetNodeName(){return m_sNodeName;}
		void SetNodeName(const tString &a_sNodeName){m_sNodeName = a_sNodeName;}

		void SetModelScale(const cVector3f &a_vScale){m_vModelScale = a_vScale;}
		cVector3f GetModelScale(){return m_vModelScale;}

		const cMatrixf &GetLocalTransform(){return m_mtxLocalTransform;}
		void SetLocalTransform(const cMatrixf &a_mtxTrans){m_mtxLocalTransform = a_mtxTrans;}

		bool GetIsOneSided() {return m_bIsOneSided;}
		const cVector3f &GetOneSidedNormal(){return m_vOneSidedNormal;}

		void Compile();
	private:
		iMaterial *m_pMaterial;
		iVertexBuffer *m_pVtxBuffer;

		cMatrixf m_mtxLocalTransform;

		tString m_sGroup;
		tString m_sNodeName;

		float *m_pVertexWeights;
		unsigned char *m_pVertexBones;

		tTriEdgeVec m_vEdges;
		tTriangleDataVec m_vTriangles;

		cVector3f m_vModelScale;

		tString m_sName;

		bool m_bDoubleSided;

		bool m_bIsOneSided;
		cVector3f m_vOneSidedNormal;

		cMaterialManager *m_pMaterialManager;

		cMesh *m_pParent;
	};
};

#endif