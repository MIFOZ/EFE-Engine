#include "scene/Light3D.h"

#include "impl/tinyXML/tinyxml.h"
#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderer3D.h"
#include "graphics/Mesh.h"
#include "graphics/RenderList.h"
#include "scene/Camera3D.h"
#include "math/Math.h"
#include "scene/MeshEntity.h"
#include "graphics/SubMesh.h"
#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/FileSearcher.h"

#include "scene/World3D.h"
#include "scene/SectorVisibility.h"
#include "scene/PortalContainer.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iLight3D::iLight3D(tString a_sName, cResources *a_pResources) : iLight(), iRenderable(a_sName)
	{
		m_bStaticCasterAdded = false;

		m_bOnlyAffectInSector = false;

		m_bApplyTransformToBV = false;

		m_pFileSearcher = a_pResources->GetFileSearcher();

		m_pVisSectorCont = NULL;
		m_lSectorVisibilityCount = -1;

		for (int i=0;i<3;++i) m_vTempTextures[i] = NULL;
	}

	iLight3D::~iLight3D()
	{
		//m_pTextureManager->Destroy(m_pFalloffMap);

		if (m_pVisSectorCont) efeDelete(m_pVisSectorCont);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iLight3D::SetVisible(bool a_bVisible)
	{
		SetRendered(a_bVisible);

		//for (size_t = 0;
	}

	//--------------------------------------------------------------

	bool iLight3D::CheckObjectIntersection(iRenderable *a_pObject)
	{
		if (m_bCastShadows)
		{
			if (m_lSectorVisibilityCount != GetMatrixUpdateCount())
			{
				m_lSectorVisibilityCount = GetMatrixUpdateCount();
				if (m_pVisSectorCont) efeDelete(m_pVisSectorCont);

				//m_pVisSectorCont = CreateSectorVisibility();
			}

			tRenderContainerDataList *pDataList = a_pObject->GetRenderContainerDataList();

			if (pDataList->empty())
				return CollidesWithBV(a_pObject->GetBoundingVolume());
			else
			{
				tRenderContainerDataListIt it = pDataList->begin();
				for (;it != pDataList->end(); ++it)
				{
					cSector *pSector = static_cast<cSector*>(*it);

					cSectorVisibility *pVisSector = m_pVisSectorCont->GetSectorVisibility(pSector);
					if (pVisSector)
					{
						if (pVisSector->IntersectionBV(a_pObject->GetBoundingVolume()))
							return true;
					}
				}

				return false;
			}
		}

		else
			return CollidesWithBV(a_pObject->GetBoundingVolume());
	}

	//--------------------------------------------------------------

	bool iLight3D::BeginDraw(cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics)
	{
		cRectl ClipRect;
		bool bVisible = CreateClipRect(ClipRect, a_pRenderSettings, a_pLowLevelGraphics);

		if (bVisible)
		{
			a_pLowLevelGraphics->SetScissorsActive(true);
			a_pLowLevelGraphics->SetScissorRect(ClipRect);

			if (a_pRenderSettings->m_bLog)
				Log("Cliprect pos: (%d, %d) size: (%d, %d)\n",ClipRect.x, ClipRect.y, ClipRect.w, ClipRect.h);
		}
		else
		{
			if (a_pRenderSettings->m_bLog)
				Log("Cliprect entire screen\n");
		}

		if (m_bCastShadows && a_pRenderSettings->m_ShowShadows != eRendererShowShadows_None
			&& a_pRenderSettings->m_pVtxExtrudeProgram != NULL)
		{
			m_pIndexArray = a_pRenderSettings->m_pTempIndexArray;

			a_pLowLevelGraphics->SetStencilActive(true);

			a_pLowLevelGraphics->SetClearStencilActive(true);
			a_pLowLevelGraphics->SetClearDepthActive(false);
			a_pLowLevelGraphics->SetClearColorActive(false);

			a_pLowLevelGraphics->SetClearStencil(0);

			//a_pLowLevelGraphics->ClearScreen();

			a_pLowLevelGraphics->SetClearStencilActive(false);
			a_pLowLevelGraphics->SetClearDepthActive(true);
			a_pLowLevelGraphics->SetClearColorActive(true);
		}

		a_pRenderSettings->m_bMatrixWasNULL = false;

		return true;
	}

	void iLight3D::EndDraw(cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics)
	{
		a_pLowLevelGraphics->SetScissorsActive(false);
		a_pLowLevelGraphics->SetStencilActive(false);
	}

	//--------------------------------------------------------------

	void iLight3D::SetFarAttenuation(float a_fX)
	{
		m_fFarAttenuation = a_fX;

		m_bUpdateBoundingVolume = true;

		SetTransformUpdated();
	}

	//--------------------------------------------------------------

	void iLight3D::SetNearAttenuation(float a_fX)
	{
		m_fNearAttenuation = a_fX;
		if (m_fNearAttenuation>m_fFarAttenuation)
			SetFarAttenuation(m_fNearAttenuation);
	}

	//--------------------------------------------------------------

	cVector3f iLight3D::GetLightPosition()
	{
		return GetWorldPosition();
	}

	void iLight3D::UpdateLogic(float a_fTimeStep)
	{
		UpdateLight(a_fTimeStep);
		if (m_fFadeTime>0 || m_bFlickering)
		{
			m_bUpdateBoundingVolume = true;
			
			SetTransformUpdated();
		}
	}

	//--------------------------------------------------------------

	cBoundingVolume *iLight3D::GetBoundingVolume()
	{
		if (m_bUpdateBoundingVolume)
		{
			UpdateBoundingVolume();
			m_bUpdateBoundingVolume = false;
		}

		return &m_BoundingVolume;
	}

	//--------------------------------------------------------------

	cMatrixf *iLight3D::GetModelMatrix(cCamera3D *a_pCamera)
	{
		mtxTemp = GetWorldMatrix();
		return &mtxTemp;
	}

	//--------------------------------------------------------------

	void iLight3D::LoadXMLProperties(const tString a_sFile)
	{
		tString sPath = m_pFileSearcher->GetFilePath(a_sFile);
		if (sPath != "")
		{
			TiXmlDocument *pDoc = efeNew(TiXmlDocument, (sPath.c_str()));
			if (pDoc->LoadFile())
			{
				TiXmlElement *pRootElem = pDoc->RootElement();

				TiXmlElement *pMainElem = pRootElem->FirstChildElement("MAIN");
				if (pMainElem != NULL)
				{
					m_bCastShadows = cString::ToBool(pMainElem->Attribute("CastsShadows"), m_bCastShadows);

					m_DiffuseColor.a = cString::ToFloat(pMainElem->Attribute("Specular"), m_DiffuseColor.a);

					tString sFalloffImage = cString::ToString(pMainElem->Attribute("FalloffImage"), "");
					//iTexture *pTexture = m_pTextureManager->Create2D
				}
				else
					Error("Couldn't find main element in %s\n", a_sFile.c_str());
			}
			else
				Error("Couldn't load file '%s'\n", a_sFile.c_str());

			efeDelete(pDoc);
		}
		else
			Error("Couldn't find file '%s'\n", a_sFile.c_str());
	}

	//--------------------------------------------------------------

	bool iLight3D::IsVisible()
	{
		if (m_DiffuseColor.r <= 0 && m_DiffuseColor.g <= 0 && m_DiffuseColor.b <= 0 && m_DiffuseColor.a <= 0)
			return false;
		if (m_fFarAttenuation <= 0) return false;

		return IsRendered();
	}

	//--------------------------------------------------------------

	void iLight3D::OnFlickerOff()
	{
		if (m_sFlickerOffPS!=""){}
	}

	void iLight3D::OnFlickerOn()
	{
		
	}

	void iLight3D::OnSetDiffuse()
	{
		
	}
}