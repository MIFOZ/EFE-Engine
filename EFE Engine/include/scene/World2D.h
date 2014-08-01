#ifndef EFE_WORLD2D_H
#define EFE_WORLD2D_H

#include <map>

#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "math/MathTypes.h"
#include "game/GameTypes.h"

//#include "scene/SoundSource.h"

class TiXmlElement;

namespace efe
{
	class cGraphics;
	class cResources;
	class cSound;

	class iCamera;
	class cCamera2D;
	class cTileMap;
	class cTile;
	class cNode2D;
	class cBody2D;
	class cCollider2D;
	class cGridMap2D;
	class cLight2DPoint;
	class iLght2D;
	class cImageEntity;

	class cArea2D;
	class iScript;
	class cMesh2D;

	class cWorld2D
	{
		friend class cCollider2D;
	public:
		cWorld2D(tString a_sName, cGraphics *a_pGraphics, cResources *a_pResources, cSound *a_pSound, cCollider2D *a_pCollider);
		~cWorld2D();

		tString GetName(){return m_sName;}

		bool CreateFromFile(tString a_sFile);

		void Render(cCamera2D *a_pCamera);

		void Update(float a_fTimeStep);

		cVector2f GetWorldSize() {return m_vWorldSize;}

		///// LIGHT METHODS
		cGridMap2D *GetGridMapLights();
	private:
		tString m_sName;
		cGraphics *m_pGraphics;
		cSound *m_pSound;
		cResources *m_pResorces;
		cCollider2D *m_pCollider;

		cVector2f m_vWorldSize;

		iScript *m_pScirpt;

		cGridMap2D *m_pMapLights;
		cGridMap2D *m_pMapImageEnities;
		cGridMap2D *m_pMapBodies;

		int m_lBodyIDCount;

		cTileMap *m_pTileMap;
		cNode2D *m_pRootNode;

		tString m_sMapName;
		float m_fLightZ;
		cColor m_AmbientColor;

		void UpdateEntities();
		void UpdateBodies();

		void RenderImagesEntites(cCamera2D *a_pCamera);

		int LoadTileData(cTile *a_pTile, tString *a_sData, int a_lStart);
	};
};
#endif