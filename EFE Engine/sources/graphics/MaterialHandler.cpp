#include "graphics/MaterialHandler.h"

#include "graphics/Graphics.h"
#include "resources/Resources.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMaterialHandler::cMaterialHandler(cGraphics *a_pGraphics, cResources *a_pResources)
	{
		m_pGraphics = a_pGraphics;
		m_pResources = a_pResources;
	}

	cMaterialHandler::~cMaterialHandler()
	{
		tMaterialTypeListIt it = m_lstMatTypes.begin();
		for (;it != m_lstMatTypes.end(); ++it)
			efeDelete(*it);

		m_lstMatTypes.clear();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cMaterialHandler::Add(iMaterialType *a_pTypedata)
	{
		m_lstMatTypes.push_back(a_pTypedata);
	}

	//--------------------------------------------------------------

	iMaterial *cMaterialHandler::Create(tString a_sMatName, eMaterialPicture m_PicType)
	{
		return Create("", a_sMatName, m_PicType);
	}

	//--------------------------------------------------------------

	iMaterial *cMaterialHandler::Create(const tString &a_sName, tString a_sMatName, eMaterialPicture m_PicType)
	{
		iMaterial *pMat=NULL;
		cResourceImage *pImage=NULL;
		unsigned int lIdCount = 1;

		for (tMaterialTypeListIt it = m_lstMatTypes.begin(); it != m_lstMatTypes.end(); ++it)
		{
			if ((*it)->IsCorrect(a_sMatName))
			{
				pMat = (*it)->Create(a_sName, m_pGraphics->GetLowLevel(), m_pResources->GetImageManager(),
									m_pResources->GetTextureManager(),
									m_pGraphics->GetRenderer2D(),m_pResources->GetGpuProgramManager(),
									m_PicType,m_pGraphics->GetRenderer3D());

				pMat->SetId(lIdCount);

				break;
			}

			lIdCount++;
		}

		return pMat;
	}
}