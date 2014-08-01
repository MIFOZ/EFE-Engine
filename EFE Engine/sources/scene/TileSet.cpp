#include <assert.h>
#include "scene/TileSet.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "graphics/MeshCreator.h"
#include "impl/tinyXML/tinyxml.h"
#include "graphics/MaterialHandler.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cTileSet::cTileSet(tString a_sName, cGraphics *a_pGraphics, cResources *a_pResources)
		: iResourceBase(a_sName, 0)
	{
		m_pResources = a_pResources;
		m_fTileSize = 0;
		m_pGraphics = a_pGraphics;

		for (int i=0;i<eMaterialTexture_LastEnum;i++)
			m_vImageHandle[i] = -1;
	}

	//--------------------------------------------------------------

	cTileSet::~cTileSet()
	{
		Log("Deleting tileset.\n");

		for (tTileDataVecIt it = m_vData.begin(); it!=m_vData.end(); it++)
		{
			(*it)->Destroy();
			efeDelete(*it);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cTileSet::Add(iTileData *a_pData)
	{
		m_vData.push_back(a_pData);
	}

	//--------------------------------------------------------------

	iTileData *cTileSet::Get(int a_lNum)
	{
		assert(a_lNum>=0 && a_lNum<(int)m_vData.size());
			
		return m_vData[a_lNum];
	}

	//--------------------------------------------------------------

	bool cTileSet::CreateFromFile(const tString &a_sFile)
	{
		TiXmlDocument *pDoc = efeNew(TiXmlDocument, (a_sFile.c_str()));
		if (!pDoc->LoadFile())
		{
			FatalError("Couldn't load tielset '%s'!\n", a_sFile.c_str());
			return false;
		}

		GetTileNum(pDoc->RootElement()->FirstChildElement());

		m_pResources->AddResourceDir(pDoc->RootElement()->Attribute("dir"));

		m_fTileSize = (float)cString::ToInt(pDoc->RootElement()->Attribute("size"),0);

		double x = ceil(log((double)((float)m_lNum)*m_fTileSize)/log(2.0f));
		double y = ceil(log((double)m_fTileSize)/log(2.0f));

		if (x>kMaxTileFrameWidth)
		{
			y += x - kMaxTileFrameWidth;
			x = kMaxTileFrameWidth;
		}

		m_vFrameSize = cVector2l((int)pow(2.0,x),(int)pow(2.0,y));

		TiXmlElement *pTileElement = pDoc->RootElement()->FirstChildElement();
		while(pTileElement!=NULL)
		{
			LoadData(pTileElement);

			pTileElement = pTileElement->NextSiblingElement();
		}

		efeDelete(pDoc);

		m_pResources->GetImageManager()->FlushAll();

		return true;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cTileSet::LoadData(TiXmlElement *a_pElement)
	{
		tString sName = a_pElement->Attribute("name");
		tString sMaterial = a_pElement->Attribute("material");
		tString sMesh = a_pElement->Attribute("mesh");
		tString sCollideMesh = cString::ToString(a_pElement->Attribute("collidemesh"),"");

		iMaterial *pMaterial = NULL;
		cMesh2D *pMesh = NULL;
		cMesh2D *pCollideMesh = NULL;
		cResourceImage *pImage = NULL;

		cTileDataNormal *pTileData = efeNew(cTileDataNormal, (m_pResources->GetImageManager(), m_fTileSize));

		pMaterial = m_pGraphics->GetMaterialHandler()->Create(sMaterial, eMaterialPicture_Image);
		if (pMaterial==NULL)
		{
			Error("Error creating material '%s' for '%s'!\n", sMaterial.c_str(), sName.c_str());
			return false;
		}

		tTextureTypeList lstTypes = pMaterial->GetTextureTypes();
		for (tTextureTypeListIt it = lstTypes.begin();it!=lstTypes.end();it++)
		{
			if (m_vImageHandle[it->m_Type]==-1)
				m_vImageHandle[it->m_Type] = m_pResources->GetImageManager()->CreateFrame(m_vFrameSize);

			pImage = m_pResources->GetImageManager()->CreateImage(sName + it->m_sSuffix,m_vImageHandle[it->m_Type]);
			if (pImage==NULL)
			{
				Error("Can't load texture '%s%s'!\n", sName.c_str(), it->m_sSuffix.c_str());
				return false;
			}

			pMaterial->SetImage(pImage, it->m_Type);
		}

		pMaterial->Compile();

		pMesh = m_pGraphics->GetMeshCreator()->Create2D(sMesh, m_fTileSize);
		if (pMesh==NULL)
		{
			Error("Error creating mesh for '%s'!\n", sName.c_str());
			return false;
		}

		pTileData->SetData(pMesh,pMaterial);

		if (sCollideMesh != "")
		{
			pCollideMesh = m_pGraphics->GetMeshCreator()->Create2D(sCollideMesh, m_fTileSize);
			if (pCollideMesh == NULL)
			{
				Error("Error creating collide mesh for '%s'!\n", sName.c_str());
				return false;
			}
			pCollideMesh->CreateTileVertexVec();

			pTileData->SetCollideMesh(pCollideMesh);
		}

		pTileData->SetIsSolid(cString::ToInt(a_pElement->Attribute("solid"), 1)?true:false);

		eTilelCollisionType CType = (eTilelCollisionType)cString::ToInt(a_pElement->Attribute("collisiontype"),1);
		pTileData->SetCollisionType(CType);

		Add(pTileData);

		return true;
	}

	//--------------------------------------------------------------

	void cTileSet::GetTileNum(TiXmlElement *a_pElement)
	{
		m_lNum = 0;
		while(a_pElement!=NULL)
		{
			m_lNum++;

			a_pElement = a_pElement->NextSiblingElement();
		}
	}
}