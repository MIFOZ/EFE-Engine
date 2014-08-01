#include "graphics/RenderList.h"
#include "graphics/Renderable.h"
#include "scene/MeshEntity.h"
#include "graphics/Material.h"
#include "scene/Camera3D.h"
#include "math/Math.h"
#include "graphics/RenderState.h"
#include "graphics/Renderer3D.h"

#include "graphics/Graphics.h"

namespace efe
{
	int cRenderList::m_lGlobalRenderCount = 0;


	//////////////////////////////////////////////////////////////
	// TRANSPARENT OBJECT COMPARE
	//////////////////////////////////////////////////////////////

	bool cTransparentObject_Compare::operator()(iRenderable *a_pObjectA, iRenderable *a_pObjectB) const
	{
		if (a_pObjectA->GetMaterial()->GetDepthTest() != a_pObjectB->GetMaterial()->GetDepthTest())
			return a_pObjectA->GetMaterial()->GetDepthTest() < a_pObjectB->GetMaterial()->GetDepthTest();

		return a_pObjectA->GetZ() < a_pObjectB->GetZ();
	}

	

	//////////////////////////////////////////////////////////////
	// RENDER NODE
	//////////////////////////////////////////////////////////////

	cMemoryPool<iRenderState> *g_poolRenderState = NULL;
	cMemoryPool<cRenderNode> *g_poolRenderNode = NULL;

	//--------------------------------------------------------------

	bool cRenderNodeCompare::operator()(cRenderNode *a_pNodeA, cRenderNode *a_pNodeB) const
	{
		int val = a_pNodeA->m_pState->Compare(a_pNodeB->m_pState);
		bool ret = val>0 ? true : false;
		return ret;
	}

	//--------------------------------------------------------------

	void cRenderNode::DeleteChildren()
	{
		tRenderNodeSetIt it = m_setNodes.begin();
		for (;it!=m_setNodes.end();++it)
		{
			cRenderNode *pNode = *it;

			pNode->DeleteChildren();
			g_poolRenderNode->Release(pNode);
		}

		if (m_pState) g_poolRenderState->Release(m_pState);
		m_pState = NULL;
		m_setNodes.clear();
	}

	void cRenderNode::Render(cRenderSettings *a_pSettings)
	{
		tRenderNodeSetIt it = m_setNodes.begin();
		for (;it!=m_setNodes.end();++it)
		{
			cRenderNode *pNode = *it;
			pNode->m_pState->SetMode(a_pSettings);
			pNode->Render(a_pSettings);
		}
	}

	cRenderList::cRenderList(cGraphics *a_pGraphics)
	{
		m_fFrameTime = 0;
		m_TempNode.m_pState = efeNew(iRenderState, ());

		m_lRenderCount = 0;
		m_lLastRenderCount = 0;

		m_pGraphics = a_pGraphics;

		m_poolRenderState = efeNew(cMemoryPool<iRenderState>, (3000, NULL));
		m_poolRenderNode = efeNew(cMemoryPool<cRenderNode>, (3000, NULL));

		g_poolRenderState = m_poolRenderState;
		g_poolRenderNode = m_poolRenderNode;
	}

	cRenderList::~cRenderList()
	{
		Clear();
		efeDelete(m_TempNode.m_pState);

		efeDelete(m_poolRenderState);
		efeDelete(m_poolRenderNode);

		g_poolRenderState = NULL;
		g_poolRenderNode = NULL;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cRenderList::Clear()
	{
		++m_lGlobalRenderCount;

		m_setLights.clear();
		m_setObjects.clear();

		m_RootNodeDepth.DeleteChildren();
		m_RootNodeDiffuse.DeleteChildren();
		m_RootNodeTrans.DeleteChildren();

		for (int i=0;i<MAX_NUM_OF_LIGHTS;i++)
			m_RootNodeLight[i].DeleteChildren();

		m_lLastRenderCount = m_lRenderCount;
		m_lRenderCount++;
	}

	//--------------------------------------------------------------

	cRenderNode *cRenderList::GetRootNode(eRenderListDrawType a_ObjectType, eMaterialRenderType a_PassType, int a_lLightNum)
	{
		if (a_ObjectType == eRenderListDrawType_Normal)
		{
			if (a_PassType == eMaterialRenderType_LinearDepthNormal) return &m_RootNodeDepth;
			if (a_PassType == eMaterialRenderType_Perspective) return &m_RootNodeDiffuse;
			if (a_PassType == eMaterialRenderType_Lights) return &m_RootNodeLight[a_lLightNum];
			return NULL;
		}
		else
			return &m_RootNodeTrans;
	}

	//--------------------------------------------------------------

	cLight3DIterator cRenderList::GetLightIt()
	{
		return cLight3DIterator(&m_setLights);
	}

	//--------------------------------------------------------------

	cRenderableIterator cRenderList::GetObjectIt()
	{
		return cRenderableIterator(&m_setObjects);
	}

	//--------------------------------------------------------------

	int cRenderList::GetLightNum()
	{
		return (int)m_setLights.size();
	}

	int cRenderList::GetObjectNum()
	{
		return (int)m_setObjects.size();
	}

	//--------------------------------------------------------------

	void cRenderList::Compile()
	{
		int lLightNum = (int)m_setLights.size();
		if (lLightNum > MAX_NUM_OF_LIGHTS) lLightNum = MAX_NUM_OF_LIGHTS;

		for (int i=0;i<lLightNum;i++) m_vObjectsPerLight[i] = 0;

		tRenderableSetIt it = m_setObjects.begin();
		for(;it != m_setObjects.end();++it)
		{
			iRenderable *pObject = *it;
			iMaterial *pMat = pObject->GetMaterial();

			if (pObject->GetRenderType() == eRenderableType_Mesh) continue;

			if (pMat->IsTransparent())
				m_setTransparentObjects.insert(pObject);
			else
			{
				if (pMat->UsesType(eMaterialRenderType_LinearDepthNormal))
				{
					for (int lPass=0;lPass<pMat->GetNumOfPasses(eMaterialRenderType_LinearDepthNormal, NULL);lPass++)
						AddToTree(pObject, eRenderListDrawType_Normal, eMaterialRenderType_LinearDepthNormal,
						0, NULL, false, lPass);
				}

				if (pMat->UsesType(eMaterialRenderType_Lights))
				{
					int lLightCount = 0;

					tLight3DSetIt lightIt = m_setLights.begin();
					for (;lightIt!=m_setLights.end();++lightIt)
					{
						iLight3D *pLight = *lightIt;

						if ((pLight->GetOnlyAffectInSector()==false || pObject->IsInSector(pLight->GetCurrentSector())) && 
							pLight->CheckObjectIntersection(pObject))
						{
							if (lLightCount >= MAX_NUM_OF_LIGHTS) break;

							++m_vObjectsPerLight[lLightCount];

							for (int lPass=0;lPass<pMat->GetNumOfPasses(eMaterialRenderType_Lights, pLight);++lPass)
								AddToTree(pObject, eRenderListDrawType_Normal,
											eMaterialRenderType_Lights, lLightCount, pLight, false, lPass);
						}
						++lLightCount;
					}
				}

				if (pMat->UsesType(eMaterialRenderType_Perspective))
				{
					for (int lPass=0;lPass<pMat->GetNumOfPasses(eMaterialRenderType_Perspective, NULL);lPass++)
						AddToTree(pObject, eRenderListDrawType_Normal,
									eMaterialRenderType_Perspective, 0, NULL, false, lPass);
				}
			}
		}
	}

	bool cRenderList::Add(iRenderable *a_pObject)
	{
		if (a_pObject->IsVisible() == false) return false;

		cRenderer3D *pRenderer = m_pGraphics->GetRenderer3D();
		//if (pRenderer-

		if (m_lRenderCount == a_pObject->GetRenderCount()) return false;
		a_pObject->SetRenderCount(m_lRenderCount);

		a_pObject->UpdateGraphics(cRenderList::GetCamera(), m_fFrameTime, this);

		a_pObject->SetGlobalRenderCount(m_lGlobalRenderCount);

		switch(a_pObject->GetRenderType())
		{
		case eRenderableType_Normal: case eRenderableType_ParticleSystem:
			{
				if (a_pObject->GetMaterial()->IsTransparent())
				{
				
				}
				else
				{
					m_setObjects.insert(a_pObject);
				}
			}
			break;
		case eRenderableType_Mesh:
			{
				cMeshEntity *pMesh = static_cast<cMeshEntity*>(a_pObject);
				for (int i=0; i<pMesh->GetSubMeshEntityNum();i++)
					Add(pMesh->GetSubMeshEntity(i));
				break;
			}
		case eRenderableType_Light:
			{
				iLight3D *pLight = static_cast<iLight3D*>(a_pObject);

				m_setLights.insert(pLight);
				break;
			}
		}

		return true;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cRenderNode *cRenderList::InsertNode(cRenderNode *a_pListNode, cRenderNode *a_pTempNode)
	{
		tRenderNodeSetIt it = a_pListNode->m_setNodes.find(a_pTempNode);

		if (it != a_pListNode->m_setNodes.end())
			return *it;
		else
		{
			cRenderNode *pNode = m_poolRenderNode->Create();
			pNode->m_pState = m_poolRenderState->Create();

			pNode->m_pState->Set(a_pTempNode->m_pState);

			a_pListNode->m_setNodes.insert(pNode);
			return pNode;
		}
	}

	void cRenderList::AddToTree(iRenderable *a_pObject, eRenderListDrawType a_ObjectType,
						eMaterialRenderType a_PassType, int a_lLightNum, iLight3D *a_pLight,
						bool a_bUseDepth, int a_lPass)
	{
		cRenderNode *pNode = GetRootNode(a_ObjectType, a_PassType, a_lLightNum);
		iMaterial *pMaterial = a_pObject->GetMaterial();
		iRenderState *pTempState = m_TempNode.m_pState;
		cRenderNode *pTempNode = &m_TempNode;

		if (a_PassType == eMaterialRenderType_LinearDepthNormal)
		{
			pTempState->m_Type = eRenderStateType_Sector;
			pTempState->m_pSector = a_pObject->GetCurrentSector();

			pNode = InsertNode(pNode, pTempNode);
		}

		/////// PASS //////////////
		{
			pTempState->m_Type = eRenderStateType_Pass;
			pTempState->m_lPass = a_lPass;

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// DEPTH TEST //////////////
		{
			pTempState->m_Type = eRenderStateType_DepthTest;
			pTempState->m_bDepthTest = pMaterial->GetDepthTest();

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// DEPTH //////////////
		if (a_bUseDepth)
		{
			pTempState->m_Type = eRenderStateType_Depth;
			pTempState->m_fZ = a_pObject->GetZ();

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// ALPHA MODE //////////////
		{
			pTempState->m_Type = eRenderStateType_AlphaMode;
			pTempState->m_AlphaMode = pMaterial->GetAlphaMode(a_PassType, a_lPass, a_pLight);

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// BLEND MODE //////////////
		{
			pTempState->m_Type = eRenderStateType_BlendMode;
			pTempState->m_BlendMode = pMaterial->GetBlendMode(a_PassType, a_lPass, a_pLight);

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// VERTEX PROGRAM //////////////
		{
			pTempState->m_Type = eRenderStateType_VertexProgram;

			pTempState->m_pVtxProgram = pMaterial->GetVertexProgram(a_PassType, a_lPass, a_pLight);
			pTempState->m_pVtxProgramSetup = pMaterial->GetVertexProgramSetup(a_PassType, a_lPass, a_pLight);
			pTempState->m_bUsesLight = pMaterial->VertexProgramUsesLight(a_PassType, a_lPass, a_pLight);
			pTempState->m_bUsesEye = pMaterial->VertexProgramUsesEye(a_PassType, a_lPass, a_pLight);
			pTempState->m_pLight = a_pLight;

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// PIXEL PROGRAM //////////////
		{
			pTempState->m_Type = eRenderStateType_PixelProgram;

			pTempState->m_pPxlProgram = pMaterial->GetPixelProgram(a_PassType, a_lPass, a_pLight);
			pTempState->m_pPxlProgramSetup = pMaterial->GetPixelProgramSetup(a_PassType, a_lPass, a_pLight);

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// TEXTURE //////////////
		{
			pTempState->m_Type = eRenderStateType_Texture;

			for (int i=0; i<(int)eMaterialTexture_LastEnum; i++)
			{
				pTempState->m_pTexture[i] = pMaterial->GetTexture((eMaterialTexture)i);
				pTempState->m_pTextureParams[i] = pMaterial->GetTextureParamsPtr((eMaterialTexture)i);
			}


// 			for (int i=0;i<MAX_TEXTUREUNITS;i++)
// 				pTempState->m_pTexture[i] = pMaterial->GetTexture(i, a_PassType, a_lPass, a_pLight);

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// SAMPLER //////////////
		{
// 			pTempState->m_Type = eRenderStateType_Sampler;
// 
// 			//for (int i=0;i<MAX_TEXTUREUNITS;i++)
// 			//	pTempState->m_TextureAddressMode[i] = pMaterial->GetTexture(i, a_PassType, a_lPass, a_pLight);
// 
// 			pNode = InsertNode(pNode, pTempNode);
		}
		/////// VERTEX BUFFER //////////////
		{
			pTempState->m_Type = eRenderStateType_VertexBuffer;

			pTempState->m_pVtxBuffer = a_pObject->GetVertexBuffer();

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// MATRIX //////////////
		{
			pTempState->m_Type = eRenderStateType_Matrix;

			pTempState->m_pModelMatrix = a_pObject->GetModelMatrix(m_pCamera);
			pTempState->m_pInvModelMatrix = a_pObject->GetInvModelMatrix();
			pTempState->m_vScale = a_pObject->GetCalcScale();

			pNode = InsertNode(pNode, pTempNode);
		}
		/////// RENDER //////////////
		{
			pTempState->m_Type = eRenderStateType_Render;

			pTempState->m_pObject = a_pObject;
			pTempState->m_pRenderEffect = pMaterial->GetRenderEffect(eMaterialRenderType_Perspective);

			pNode = InsertNode(pNode, pTempNode);
		}
	}
}