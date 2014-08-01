#include "graphics/GraphicsDrawer.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/ResourceImage.h"
#include "resources/FrameBitmap.h"
#include "graphics/GfxObject.h"
#include "graphics/MaterialHandler.h"
#include "graphics/Material.h"
#include "resources/TextureManager.h"

#include "math/Math.h"

#include "resources/Resources.h"
#include "resources/ImageManager.h"

namespace efe
{
	cGraphicsDrawer::cGraphicsDrawer(iLowLevelGraphics *a_pLowLevelGraphics, cMaterialHandler *a_pMaterialHandler,
							cResources *a_pResources)
	{
		m_pLowLevelGraphics = a_pLowLevelGraphics;
		m_pMaterialHandler = a_pMaterialHandler;
		m_pResources = a_pResources;
	}

	cGraphicsDrawer::~cGraphicsDrawer()
	{
		ClearBackgrounds();
	}

	iMaterial *cGfxBufferObject::GetMaterial() const
	{
		return m_pObject->GetMaterial();
	}

	bool cGfxBufferCompare::operator()(const cGfxBufferObject &a_ObjectA, const cGfxBufferObject &a_ObjectB)
	{
		if (a_ObjectA.GetZ() != a_ObjectB.GetZ())
		{
			return a_ObjectA.GetZ() < a_ObjectB.GetZ();
		}
		else if (a_ObjectA.GetMaterial()->GetTexture(eMaterialTexture_Diffuse) !=
			a_ObjectB.GetMaterial()->GetTexture(eMaterialTexture_Diffuse))
		{
			return a_ObjectA.GetMaterial()->GetTexture(eMaterialTexture_Diffuse) >
				a_ObjectB.GetMaterial()->GetTexture(eMaterialTexture_Diffuse);
		}
		else if (a_ObjectA.GetMaterial()->GetType(eMaterialRenderType_Perspective) !=
			a_ObjectB.GetMaterial()->GetType(eMaterialRenderType_Perspective))
		{
			return a_ObjectA.GetMaterial()->GetType(eMaterialRenderType_Perspective) >
				a_ObjectB.GetMaterial()->GetType(eMaterialRenderType_Perspective);
		}
		else
			return false;
	}

	static void FlushImage(cGfxObject *a_pObject)
	{
		if (a_pObject->IsImage())
		{
			cResourceImage *pImage = a_pObject->GetMaterial()->GetImage(eMaterialTexture_Diffuse);
			pImage->GetFrameBitmap()->FlushToTexture();
		}
	}

	void cGraphicsDrawer::DrawGfxObject(cGfxObject *a_pObject, const cVector3f &a_vPos)
	{
		FlushImage(a_pObject);

		cGfxBufferObject BuffObj;
		BuffObj.m_pObject = a_pObject;
		BuffObj.m_vTransform = a_vPos;
		BuffObj.m_bIsColorAndSize = false;

		m_setGfxBuffer.insert(BuffObj);
	}

	//--------------------------------------------------------------

	void cGraphicsDrawer::DrawGfxObject(cGfxObject *a_pObject, const cVector3f &a_vPos,
											const cVector2f &a_vSize, const cColor &a_Color,
											bool a_bFlipH, bool a_bFlipV, float a_fAngle)
	{
		FlushImage(a_pObject);

		cGfxBufferObject BuffObj;
		BuffObj.m_pObject = a_pObject;
		BuffObj.m_vTransform = a_vPos;
		BuffObj.m_vSize = a_vSize;
		BuffObj.m_Color = a_Color;
		BuffObj.m_bFlipH = a_bFlipH;
		BuffObj.m_bFlipV = a_bFlipV;
		BuffObj.m_fAngle = a_fAngle;

		BuffObj.m_bIsColorAndSize = true;

		m_setGfxBuffer.insert(BuffObj);
	}

	//--------------------------------------------------------------

	void cGraphicsDrawer::DrawAll()
	{
		//m_pLowLevelGraphics->SetDepthTestActive(false);
		m_pLowLevelGraphics->Set2DMode();

		int lIdxAdd = 0;
		iMaterial *pPrevMat = NULL;
		iMaterial *pMat = NULL;
		const cGfxBufferObject *pObj = NULL;
		tGfxBufferSetIt ObjectIt = m_setGfxBuffer.begin();

		if (ObjectIt != m_setGfxBuffer.end())
			pMat = ObjectIt->GetMaterial();

		while(ObjectIt != m_setGfxBuffer.end())
		{
			if (pMat->StartRendering(eMaterialRenderType_Perspective, NULL, NULL)==false)
			{
				ObjectIt++;
				if (ObjectIt != m_setGfxBuffer.end()) pMat = ObjectIt->GetMaterial();

				continue;
			}

			do
			{
				pObj = &(*ObjectIt);
				if (pObj->m_bIsColorAndSize)
				{
					cVector3f vPos[4];
					float fW = pObj->m_vSize.x*0.5f;
					float fH = pObj->m_vSize.y*0.5f;
					cMatrixf mtxTrans = cMath::MatrixTranslate(pObj->m_vTransform + cVector3f(fW,fH,0));
					vPos[0] = cVector3f(-fW,-fH,0);
					vPos[1] = cVector3f(fW,-fH,0);
					vPos[2] = cVector3f(fW,fH,0);
					vPos[3] = cVector3f(-fW,fH,0);

					if (pObj->m_fAngle != 0)
					{
						cMatrixf mtxRot = cMath::MatrixRotateZ(pObj->m_fAngle);
						vPos[0] = cMath::MatrixMul(mtxRot,vPos[0]);
						vPos[1] = cMath::MatrixMul(mtxRot,vPos[1]);
						vPos[2] = cMath::MatrixMul(mtxRot,vPos[2]);
						vPos[3] = cMath::MatrixMul(mtxRot,vPos[3]);
					}

					vPos[0] = cMath::MatrixMul(mtxTrans,vPos[0]);
					vPos[1] = cMath::MatrixMul(mtxTrans,vPos[1]);
					vPos[2] = cMath::MatrixMul(mtxTrans,vPos[2]);
					vPos[3] = cMath::MatrixMul(mtxTrans,vPos[3]);

					if (pObj->m_bFlipH)
					{
						m_pLowLevelGraphics->AddVertexToBatch_Size2D(pObj->m_pObject->GetVtxPtr(0),
							&vPos[0],
							&pObj->m_Color,
							0,0);

						m_pLowLevelGraphics->AddVertexToBatch_Size2D(pObj->m_pObject->GetVtxPtr(1),
							&vPos[1],
							&pObj->m_Color,
							0,0);

						m_pLowLevelGraphics->AddVertexToBatch_Size2D(pObj->m_pObject->GetVtxPtr(2),
							&vPos[2],
							&pObj->m_Color,
							0,0);

						m_pLowLevelGraphics->AddVertexToBatch_Size2D(pObj->m_pObject->GetVtxPtr(3),
							&vPos[3],
							&pObj->m_Color,
							pObj->m_vSize.x,pObj->m_vSize.y);
					}
					else
					{
						m_pLowLevelGraphics->AddVertexToBatch_Size2D(pObj->m_pObject->GetVtxPtr(0),
							&vPos[0],
							&pObj->m_Color,
							0,0);

						m_pLowLevelGraphics->AddVertexToBatch_Size2D(pObj->m_pObject->GetVtxPtr(1),
							&vPos[1],
							&pObj->m_Color,
							0,0);

						m_pLowLevelGraphics->AddVertexToBatch_Size2D(pObj->m_pObject->GetVtxPtr(2),
							&vPos[2],
							&pObj->m_Color,
							0,0);

						m_pLowLevelGraphics->AddVertexToBatch_Size2D(pObj->m_pObject->GetVtxPtr(3),
							&vPos[3],
							&pObj->m_Color,
							0,0);
					}
				}
				else
				{
					for (int i=0;i<(int)pObj->m_pObject->GetVertexVec()->size();i++)
					{
						m_pLowLevelGraphics->AddVertexToBatch(pObj->m_pObject->GetVtxPtr(i), &pObj->m_vTransform);
					}
				}

				m_pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 3);
				m_pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 0);
				m_pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 2);
				m_pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 1);
				m_pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 2);
				m_pLowLevelGraphics->AddIndexToBatch(lIdxAdd + 0);

				lIdxAdd += (int)pObj->m_pObject->GetVertexVec()->size();

				pPrevMat = pMat;
				ObjectIt++;

				if (ObjectIt == m_setGfxBuffer.end())
				{
					pMat = NULL;
					break;
				}
				else
					pMat = ObjectIt->GetMaterial();
			}
			while(pMat->GetType(eMaterialRenderType_Perspective) == 
				pPrevMat->GetType(eMaterialRenderType_Perspective)
				&&
				pMat->GetTexture(eMaterialTexture_Diffuse) == 
				pPrevMat->GetTexture(eMaterialTexture_Diffuse));

			lIdxAdd = 0;

			/*do
			{*/
			m_pLowLevelGraphics->SetBatchTexture(pPrevMat->GetTexture(eMaterialTexture_Diffuse));
			m_pLowLevelGraphics->FlushQuadBatch(false);
			/*}
			while (pPrevMat*/

			m_pLowLevelGraphics->ClearBatch();

			//pPrevMat->end
		}

		m_setGfxBuffer.clear();


	}

	//--------------------------------------------------------------

	cGfxObject *cGraphicsDrawer::CreateGfxObject(const tString &a_sFileName, const tString &a_sMaterialName,
									bool a_bAddToList)
	{
		cResourceImage *pImage = m_pResources->GetImageManager()->CreateImage(a_sFileName);
		if (pImage==NULL)
		{
			FatalError("Couldn't create image");
			return false;
		}

		iMaterial *pMat = pMat = m_pMaterialHandler->Create(a_sMaterialName, eMaterialPicture_Image);
		if (pMat==NULL)
		{
			FatalError("Couldn't create material '%s'!\n", a_sMaterialName.c_str());
			return NULL;
		}

		pMat->SetImage(pImage, eMaterialTexture_Diffuse);

		cGfxObject *pObject = efeNew(cGfxObject, (pMat,"",true));

		if (a_bAddToList) m_lstGfxObjects.push_back(pObject);

		return pObject;
	}

	//--------------------------------------------------------------

	cGfxObject *cGraphicsDrawer::CreateGfxObject(iBitmap2D *a_pBmp, const tString &a_sMaterialName,
									bool a_bAddToList)
	{
		cResourceImage *pImage = m_pResources->GetImageManager()->CreateFromBitmap("",a_pBmp);
		if (pImage==NULL)
		{
			FatalError("Couldn't create image");
			return false;
		}

		iMaterial *pMat;
		pMat = m_pMaterialHandler->Create(a_sMaterialName, eMaterialPicture_Image);
		if (pMat==NULL)
		{
			FatalError("Couldn't create material '%s'!\n", a_sMaterialName.c_str());
			return NULL;
		}

		pMat->SetImage(pImage, eMaterialTexture_Diffuse);

		cGfxObject *pObject = efeNew(cGfxObject, (pMat,"",true));

		if (a_bAddToList) m_lstGfxObjects.push_back(pObject);

		return pObject;
	}

	//--------------------------------------------------------------

	cGfxObject *cGraphicsDrawer::CreateGfxObjectFromTexture(const tString &a_sFileName, const tString &a_sMaterialName,
									bool a_bAddToList)
	{
		iTexture *pTex = m_pResources->GetTextureManager()->Create2D(a_sFileName, false);
		if (pTex==NULL)
		{
			FatalError("Couldn't create texture '%s'!\n", a_sFileName.c_str());
			return NULL;
		}

		iMaterial *pMat = m_pMaterialHandler->Create(a_sMaterialName.c_str(), eMaterialPicture_Texture);
		if (pMat==NULL)
		{
			FatalError("Couldn't create material '%s'!\n", a_sMaterialName.c_str());
			return NULL;
		}

		pMat->SetTexture(pTex, eMaterialTexture_Diffuse);

		cGfxObject *pObject = efeNew(cGfxObject, (pMat, a_sFileName, false));

		if (a_bAddToList) m_lstGfxObjects.push_back(pObject);

		return pObject;
	}

	//--------------------------------------------------------------

	cBackgroundImage *cGraphicsDrawer::AddBackgroundImage(const tString &a_sFileName, const tString &a_sMaterialName,
							const cVector3f &a_vPos, bool a_bTile,
							const cVector2f &a_vSize, const cVector2f &a_vPosPercent, const cVector2f &a_vVel)
	{
		cResourceImage *pImage = m_pResources->GetImageManager()->CreateImage(a_sFileName);
		if (pImage==NULL)
		{
			FatalError("Couldn't load image '%s'\n!", a_sFileName.c_str());
			return NULL;
		}

		iMaterial *pMat = m_pMaterialHandler->Create(a_sMaterialName, eMaterialPicture_Image);
		if (pMat==NULL)
		{
			FatalError("Couldn't create material '%s'\n!", a_sMaterialName.c_str());
			return NULL;
		}

		pMat->SetImage(pImage, eMaterialTexture_Diffuse);

		cBackgroundImage *pBG = efeNew(cBackgroundImage, (pMat,a_vPos,a_bTile,a_vSize,a_vPosPercent,a_vVel));

		m_mapBackgroundImages.insert(tBackgroundImageMap::value_type(a_vPos.z, pBG));

		return pBG;
	}

	//--------------------------------------------------------------

	void cGraphicsDrawer::UpdateBackgrounds()
	{
		tBackgroundImageMapIt it = m_mapBackgroundImages.begin();
		for (;it!=m_mapBackgroundImages.end();it++)
			it->second->Update();
	}

	//--------------------------------------------------------------

	void cGraphicsDrawer::DrawBackgrounds(const cRectf &a_CollideRect)
	{
		tBackgroundImageMapIt it = m_mapBackgroundImages.begin();
		for (;it!=m_mapBackgroundImages.end();it++)
			it->second->Draw(a_CollideRect, m_pLowLevelGraphics);
	}

	//--------------------------------------------------------------

	void cGraphicsDrawer::ClearBackgrounds()
	{
		tBackgroundImageMapIt it = m_mapBackgroundImages.begin();
		for (;it!=m_mapBackgroundImages.end();it++)
			efeDelete(it->second);
		m_mapBackgroundImages.clear();
	}
}