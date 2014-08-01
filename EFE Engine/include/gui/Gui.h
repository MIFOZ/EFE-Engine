#ifndef EFE_GUI_H
#define EFE_GUI_H

#include <map>

#include "game/Updateable.h"

#include "gui/GuiTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	class cResources;
	class cGraphics;
	class cSound;
	class cScene;

	class cGuiSet;
	class cGuiSkin;
	class cGuiGfxElement;

	class iGuiMaterial;

	//-----------------------------------------------------------

	typedef std::map<tString, cGuiSet*> tGuiSetMap;
	typedef tGuiSetMap::iterator tGuiSetMapIt;

	//-----------------------------------------------------------

	typedef std::map<tString, cGuiSkin*> tGuiSkinMap;
	typedef tGuiSkinMap::iterator tGuiSkinMapIt;

	//-----------------------------------------------------------

	typedef std::map<tString, eGuiSkinGfx> tGuiSkinGfxMap;
	typedef tGuiSkinGfxMap::iterator tGuiSkinGfxMapIt;

	typedef std::map<tString, eGuiSkinFont> tGuiSkinFontMap;
	typedef tGuiSkinFontMap::iterator tGuiSkinFontMapIt;

	typedef std::map<tString, eGuiSkinAttribute> tGuiSkinAttributeMap;
	typedef tGuiSkinAttributeMap::iterator tGuiSkinAttributeMapIt;

	//-----------------------------------------------------------

	class cGui : public iUpdateable
	{
	public:
		cGui();
		~cGui();

		//General
		void Init(cResources *a_pResources, cGraphics *a_pGraphics,
			cSound *a_pSound, cScene *a_pScene);

		void Update(float a_fTimeStep);

		void OnPostSceneDraw();
		void OnPostGUIDraw();

		iGuiMaterial *GetMaterial(eGuiMaterial a_Type);

		//Skins
		cGuiSkin *CreateSkin(const tString &a_sFile);

		eGuiSkinGfx GetSkinGfxFromString(const tString &a_sType);
		eGuiSkinFont GetSkinFontFromString(const tString &a_sType);
		eGuiSkinAttribute GetSkinAttributeFromString(const tString &a_sType);

		//Sets
		cGuiSet *CreateSet(const tString &a_sName, cGuiSkin *a_pSkin);
		cGuiSet *GetSetFromName(const tString &a_sName);
		void SetFocus(cGuiSet *a_pSet);
		void SetFocusByName(const tString &a_sSetName);
		cGuiSet *GetFocusedSet(){return m_pSetInFocus;}
		void DestroySet(cGuiSet *a_pSet);

		//Graphics creation
		cGuiGfxElement *CreateGfxFilledRect(const cColor &a_Color, eGuiMaterial a_Material, bool a_bAddToList=true);
		cGuiGfxElement *CreateGfxImage(	const tString &a_sFile, eGuiMaterial a_Material,
										const cColor &a_Color=cColor(1,1), bool a_bAddToList=true);
		cGuiGfxElement *CreateGfxTexture(const tString &a_sFile, eGuiMaterial a_Material,
										const cColor &a_Color=cColor(1,1), bool a_bMipMaps=false,
										bool a_bAddToList=true);

		cGuiGfxElement *CreateGfxImageBuffer(const tString &a_sFile, eGuiMaterial a_Material,
												bool a_bCreateAnimation=true,
												const cColor &a_Color=cColor(1,1),bool a_bAddToList=true);

		void DestroyGfx(cGuiGfxElement *a_pGfx);

		//Input sending
		bool SendMousePos(const cVector2f &a_vPos, const cVector2f &a_vRel);
		bool SendMouseClickDown(eGuiMouseButton a_Button);
		bool SendMouseClickUp(eGuiMouseButton a_Button);
		bool SendMouseDoubleClick(eGuiMouseButton a_Button);

		bool SendKeyPress(const cKeyPress &keyPress);

		cResources *GetResources(){return m_pResources;}
	private:
		void GenerateSkinTypeStrings();

		cResources *m_pResources;
		cGraphics *m_pGraphics;
		cSound *m_pSound;
		cScene *m_pScene;

		cGuiSet *m_pSetInFocus;

		tGuiSetMap m_mapSets;
		tGuiSkinMap m_mapSkins;

		iGuiMaterial *m_vMaterials[eGuiMaterial_LastEnum];

		tGuiGfxElementList m_lstGfxElements;

		tGuiSkinGfxMap m_mapSkinGfxStrings;
		tGuiSkinFontMap m_mapSkinFontStrings;
		tGuiSkinAttributeMap m_mapSkinAttributeStrings;

		unsigned long m_lLastRenderTime;
	};
};

#endif