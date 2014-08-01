#include "graphics/SubMesh.h"

#include "graphics/Mesh.h"
#include "resources/MaterialManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"
#include "math/Math.h"

#include "system/MemoryManager.h"

#include <string.h>

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSubMesh::cSubMesh(const tString &a_sName, cMaterialManager *a_pMaterialManager)
	{
		m_pMaterialManager = a_pMaterialManager;

		m_sName = a_sName;
		m_sNodeName = "";

		m_pMaterial = NULL;
		m_pVtxBuffer = NULL;

		m_bDoubleSided = false;

		m_pVertexWeights = NULL;
		m_pVertexBones = NULL;

		m_mtxLocalTransform = cMatrixf::Identity;

		m_bIsOneSided = false;
		m_vOneSidedNormal = 0;
	}

	cSubMesh::~cSubMesh()
	{
		if (m_pMaterial) m_pMaterialManager->Destroy(m_pMaterial);
		if (m_pVtxBuffer) efeDelete(m_pVtxBuffer);

		if (m_pVertexBones) efeDeleteArray(m_pVertexBones);
		if (m_pVertexWeights) efeDeleteArray(m_pVertexWeights);
	}

	void cSubMesh::SetMaterial(iMaterial *a_pMaterial)
	{
		if (m_pMaterial) m_pMaterialManager->Destroy(m_pMaterial);
		m_pMaterial = a_pMaterial;

		tGpuProgramVec vVtxPrograms = m_pMaterial->GetAllVertexProgramVec();

	}

	void cSubMesh::SetVertexBuffer(iVertexBuffer *a_pVtxBuffer)
	{
		if (m_pVtxBuffer == a_pVtxBuffer) return;

		m_pVtxBuffer = a_pVtxBuffer;
	}

	//--------------------------------------------------------------

	iMaterial *cSubMesh::GetMaterial()
	{
		return m_pMaterial;
	}

	//--------------------------------------------------------------

	iVertexBuffer *cSubMesh::GetVertexBuffer()
	{
		return m_pVtxBuffer;
	}

	void cSubMesh::Compile()
	{
		
	}
}