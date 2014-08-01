#include "impl/MeshLoaderCollada.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "system/String.h"
#include "system/System.h"

#include "graphics/Mesh.h"

#include "resources/MaterialManager.h"
#include "graphics/Material.h"

#include "impl/tinyXML/tinyxml.h"

#include "math/Math.h"

namespace efe
{
	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// FILL STRUCTURES
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cMeshLoaderCollada::FillStructures(const tString &a_sFile,
							tColladaImageVec *a_pColladaImageVec,
							tColladaTextureVec *a_pColladaTextureVec,
							tColladaMaterialVec *a_pColladaMaterialVec,
							tColladaLightVec *a_pColladaLightVec,
							tColladaGeometryVec *a_pColladaGeometryVec,
							tColladaControllerVec *a_pColladaontrollerVec,
							tColladaAnimationVec *a_pColladaAnimVec,
							cColladaScene *a_pColladaScene,bool a_bCache)
	{
		bool bLoadCache = false;

		tString sCacheFile = cString::GetFileName(cString::SetFileExt(a_sFile, "collcach"));
		sCacheFile = m_sCacheDir + sCacheFile;

		if (a_bCache &&
			FileExists(cString::To16Char(sCacheFile)) &&
			FileExists(cString::To16Char(a_sFile)))
		{
			cDate colladaDate = FileModifiedDate(cString::To16Char(a_sFile));
			cDate cacheDate = FileModifiedDate(cString::To16Char(sCacheFile));

			if (cacheDate > colladaDate) bLoadCache = true;
		}

		if (bLoadCache)
		{
			//return LoadStructures()
		}

		Log("Cache  out of date! Reloadin' collada file '%s'\n", a_sFile.c_str());

		TiXmlDocument *pXmlDoc = efeNew(TiXmlDocument, (a_sFile.c_str()));
		if (pXmlDoc->LoadFile()== false)
		{
			Error("Couldn't load Collada XML file '%s'\n", a_sFile.c_str());
			efeDelete(pXmlDoc);
			return NULL;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		m_bZToY = false;

		TiXmlElement *pAssetElem = pRootElem->FirstChildElement("asset");
		if (pAssetElem)
		{
			TiXmlElement *pUpAxisElem = pAssetElem->FirstChildElement("up_axis");
			if (pUpAxisElem)
			{
				TiXmlText *pAxisText = pUpAxisElem->FirstChild()->ToText();

				if (tString(pAxisText->Value())=="Z_UP")
					m_bZToY = true;
			}
		}
		else
			Error("Couldn't find asset elemetnt!\n");

		TiXmlElement *pLibraryElem = pRootElem->FirstChildElement();
		while (pLibraryElem)
		{
			tString sType = cString::ToString(pLibraryElem->Attribute("type"),"");
			tString sValue = cString::ToString(pLibraryElem->Value(),"");

			if ((sType == "LIGHT" || sValue == "library_lights") && a_pColladaLightVec)
			{
				Log("Loading type: %s\n", sType.c_str());
				//LoadLights(pLibarayElem, a_pColladaLightVec);
				Log(" --- \n");
			}
			pLibraryElem = pLibraryElem->NextSiblingElement();
		}

		// LOAD SCENE
		if (a_pColladaScene)
		{
			TiXmlElement *pSceneElem = pRootElem->FirstChildElement("library_visual_scenes");
			if (pSceneElem)
			{
				pSceneElem = pSceneElem->FirstChildElement("visual_scene");
				if (pSceneElem==NULL) Warning("No visual scene element found!\n");
			}
			if (pSceneElem==NULL) pSceneElem = pRootElem->FirstChildElement("scene");

			if (pSceneElem==NULL)
				Warning("No scene element found!\n");
			else
			{
				TiXmlElement *pExtraElem = pSceneElem->FirstChildElement("extra");
				if (pExtraElem)
				{
					TiXmlElement *pExtraTechElem = pExtraElem->FirstChildElement("technique");
					for (;pExtraTechElem; pExtraTechElem = pExtraTechElem->NextSiblingElement("technique"))
					{
						tString sProfile = cString::ToString(pExtraTechElem->Attribute("profile"),"");
						if (sProfile == "MAYA")
						{
							//Iterate params
							TiXmlElement *pParam = pExtraTechElem->FirstChildElement();
							for (; pParam; pParam = pParam->NextSiblingElement())
							{
								tString sName = cString::ToString(pParam->Attribute("name"), "");
								if (sName == "")
								{
									sName = cString::ToString(pParam->Value(), "");
								}

								sName = cString::ToLowerCase(sName);

								TiXmlText *pText = pParam->FirstChild()->ToText();
								float fValue = cString::ToFloat(pText->Value(), 0);

								if (sName == "start_time") a_pColladaScene->m_fStartTime = fValue;
								else if (sName == "end_time") a_pColladaScene->m_fEndTime = fValue;
							}
						}
						a_pColladaScene->m_fDeltaTime = a_pColladaScene->m_fEndTime - a_pColladaScene->m_fStartTime;
					}

					Log("Anim Start: %f End: %f\n", a_pColladaScene->m_fStartTime, a_pColladaScene->m_fEndTime);
				}
				else
					Warning("No 'extra scene' element found!\n");

				TiXmlElement *pNodeElem = pSceneElem->FirstChildElement("node");
				while (pNodeElem)
				{
					LoadColladaScene(pNodeElem, &a_pColladaScene->m_Root, a_pColladaScene, a_pColladaLightVec);

					pNodeElem = pNodeElem->NextSiblingElement("node");
				}
			}
		}

		pLibraryElem = pRootElem->FirstChildElement();
		while (pLibraryElem)
		{
			tString sType = cString::ToString(pLibraryElem->Attribute("type"),"");
			tString sValue = cString::ToString(pLibraryElem->Value(),"");

			//Image
			if ((sType=="IMAGE" || sValue == "library_images") && a_pColladaImageVec)
				LoadImages(pLibraryElem, *a_pColladaImageVec);
			//Texture
			else if ((sType=="TEXTURE" || sValue == "library_effects") && a_pColladaTextureVec)
				LoadTextures(pLibraryElem, *a_pColladaTextureVec);
			//Material
			else if ((sType=="MATERIAL" || sValue == "library_materials") && a_pColladaMaterialVec)
				LoadMaterials(pLibraryElem, *a_pColladaMaterialVec);
			//Geometry
			else if ((sType=="GEOMETRY" || sValue == "library_geometries") && a_pColladaGeometryVec)
				LoadGeometry(pLibraryElem, *a_pColladaGeometryVec, a_pColladaScene);
			//Animation
			else if ((sType=="ANIMATION" || sValue == "library_animations") && a_pColladaScene && a_pColladaAnimVec)
				LoadAnimations(pLibraryElem, *a_pColladaAnimVec, a_pColladaScene);

			pLibraryElem = pLibraryElem->NextSiblingElement();
		}

		efeDelete(pXmlDoc);
		return true;
	}
}