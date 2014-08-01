#ifndef EFE_TILESET_H
#define EFE_TILESET_H

#include <vector>
#include "scene/TileData.h"
#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "resources/ResourceBase.h"

class TiXmlElement;

namespace efe
{
#define kMaxTileFrameWidth (9)

	typedef std::vector<iTileData*> tTileDataVec;
	typedef tTileDataVec::iterator tTileDataVecIt;

	class cResources;

	class cTileSet : public iResourceBase
	{
	public:
		cTileSet(tString a_sName, cGraphics *a_pGraphics, cResources *a_pResources);
		~cTileSet();

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}

		void Add(iTileData *a_pData);
		iTileData *Get(int a_lNum);

		bool CreateFromFile(const tString &a_sFile);

	private:
		float m_fTileSize;
		tTileDataVec m_vData;
		cResources *m_pResources;
		cGraphics *m_pGraphics;

		int m_lNum;
		int m_vImageHandle[eMaterialTexture_LastEnum];
		cVector2l m_vFrameSize;

		bool LoadData(TiXmlElement *a_pElement);
		void GetTileNum(TiXmlElement *a_pElement);
	};
};
#endif