#ifndef EFE_TILEDATA_H
#define EFE_TILEDATA_H

#include "graphics/Graphics.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourceImage.h"
#include "resources/ImageManager.h"
#include "graphics/Material.h"
#include "graphics/Mesh2d.h"

namespace efe
{
	enum eTilelCollisionType
	{
		eCollisionType_None,
		eCollisionType_Normal,
		eCollisionType_OnlyDown,
		eCollisionType_LastEnum
	};

	enum eTileDataType
	{
		eTileDataType_Normal,
		eTileDataType_LastEnum
	};

	class iTileData
	{
	public:
		virtual ~iTileData(){}

		virtual void Destroy()=0;
		virtual bool IsSolid()=0;
		virtual eTileDataType GetType()=0;
	};

	class cTileDataNormal : public iTileData
	{
	public:
		cTileDataNormal(cImageManager *a_pImageManager, cVector2f a_vTileSize);
		~cTileDataNormal();

		void Destroy();
		bool IsSolid(){return m_bIsSolid;}
		void SetIsSolid(bool a_bX){m_bIsSolid = a_bX;}

		eTilelCollisionType GetCollisionType() {return m_CollisionType;}
		void SetCollisionType(eTilelCollisionType a_CollisionType) {m_CollisionType = a_CollisionType;}

		eTileDataType GetType() {return eTileDataType_Normal;}
		tVertexVec *GetVertexVec(eTileRotation a_Rotation);
		tUIntVec *GetIndexVec(eTileRotation a_Rotation);

		tVertexVec *GetCollideVertexVec(eTileRotation a_Rotation);
		tUIntVec *GetCollideIndexVec(eTileRotation a_Rotation);

		iMaterial *GetMaterial(){return m_pMaterial;}

		cMesh2D *GetMesh() {return m_pMesh;}

		void SetData(cMesh2D *a_pMesh, iMaterial *a_pMaterial);

		cMesh2D *GetCollideMesh();
		void SetCollideMesh(cMesh2D *a_pCollideMesh);
	private:
		cImageManager *m_pImageManager;

		tResourceImageVec m_vImage;
		iMaterial *m_pMaterial;
		tVertexVec *m_vVtx[eTileRotation_LastEnum];
		tVertexVec *m_vCollideVtx[eTileRotation_LastEnum];
		tUIntVec *m_vIdx;
		tUIntVec *m_vCollideIdx;
		cMesh2D *m_pMesh;
		cVector2f m_vTileSize;
		bool m_bIsSolid;
		eTilelCollisionType m_CollisionType;

		cMesh2D *m_pCollideMesh;

		void SetMesh(cMesh2D *a_pMesh);
		void SetMaterial(iMaterial *a_pMaterial);
	};
};
#endif