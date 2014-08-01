#ifndef EFE_GRIDMAP2D_H
#define EFE_GRIDMAP2D_H

#include <vector>
#include <map>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace efe
{
	class iEntity2D;
	class iLowLevelGraphics;

	class cGrid2D;
	class cGridMap2D;

	typedef std::vector<cGrid2D*> tGrid2DPtrVec;
	typedef tGrid2DPtrVec::iterator tGrid2DPtrVecIt;

	class cGrid2DObject
	{
	public:
		cGrid2DObject(iEntity2D *a_pEntity, cGridMap2D *a_pGridMap, unsigned int a_lHandle);

		void Update(const cRectf &a_Rect);
		void Destoy();

		unsigned int GetHandle(){return m_lHandle;}
		iEntity2D *GetEntity(){return m_pEntity;}
		bool FirstTime(unsigned int a_lGlobalCount)
		{
			if (a_lGlobalCount<m_lCount)return false;
			m_lCount = a_lGlobalCount+1;
			return true;
		}

	private:
		cVector2l m_vPosition;
		cVector2f m_vSize;
		cVector2l m_vGridSpan;

		tGrid2DPtrVec m_vGridParents;

		iEntity2D *m_pEntity;
		cGridMap2D *m_pGridMap;

		bool m_bIsInOuter;
		bool m_bIsInGlobal;
		unsigned int m_lCount;
		int m_lHandle;
	};

	typedef std::map<int,cGrid2DObject*> tGrid2DObjectMap;
	typedef tGrid2DObjectMap::iterator tGrid2DObjectMapIt;

	class cGrid2D
	{
		friend class cGridMap2DRectIt;
	public:
		void Add(cGrid2DObject *pObject)
		{
			m_mapObjects.insert(tGrid2DObjectMap::value_type(pObject->GetHandle(),pObject));
		}

		void Erase(unsigned int a_lHandle)
		{
			m_mapObjects.erase(a_lHandle);
		}
	private:
		tGrid2DObjectMap m_mapObjects;
	};

	class cGridMap2D;
	
	class iGridMap2DIt
	{
	public:
		virtual bool HasNext()=0;
		virtual iEntity2D *Next()=0;
	};

	class cGridMap2DRectIt : public iGridMap2DIt
	{
	public:
		cGridMap2DRectIt(cGridMap2D *a_pGridMap, cVector2l a_vPos, cVector2l a_vSize);

		bool HasNext();
		iEntity2D *Next();
	private:
		cGridMap2D *m_pGridMap;

		int m_lType;
		tGrid2DObjectMapIt m_It;

		cVector2l m_vPos;
		cVector2l m_vSize;
		int m_lGridNum;
		int m_lGridAdd;
		int m_lGridRowCount;
		int m_lGridColCount;

		bool m_bUpdated;

		cGrid2DObject *m_pObject;

		void GetGridObject();
	};

	typedef std::vector<cGrid2D> tGrid2DVec;
	typedef tGrid2DVec::iterator tGrid2DVecIt;

	class cGridMap2D
	{
		friend class cGrid2DObject;
		friend class cGridMap2DRectIt;
	public:
		cGridMap2D(cVector2l a_vSize, cVector2l a_vGridSize, cVector2l a_vMaxGridSpan);
		~cGridMap2D();

		iGridMap2DIt *GetRectIterator(const cRectf &a_Rect);

		cGrid2D *GetAt(int a_lX, int a_lY);

		bool AddEntity(iEntity2D *a_pEntity);
		bool RemoveEntity(iEntity2D *a_pEntity);

		int GetMaxArraySize(){return m_vMaxGridSpan.x*m_vMaxGridSpan.y;}
		const cVector2l &GetMaxGridSpan(){return m_vMaxGridSpan;}
		const cVector2l &GetGridSize(){return m_vGridSize;}
		const cVector2l &GetGridNum(){return m_vGridNum;}

		tGrid2DObjectMap *GetAllMap(){return &m_mapAllObjects;}

		void DrawGrid(iLowLevelGraphics *a_pLowLevel, float a_fZ=100,cColor a_Col = cColor(1));
		void DrawEntityGrids(iLowLevelGraphics *a_pLowLevel,cVector2f a_vWorldPos, float a_vZ=101,cColor a_Col = cColor(1,0,1,1));
	private:
		tGrid2DVec m_vGrids;

		tGrid2DObjectMap m_mapAllObjects;

		tGrid2DObjectMap m_mapGlobalObjects;

		tGrid2DObjectMap m_mapOuterObjects;

		unsigned int m_lGlobalCount;

		unsigned int m_lHandleCount;

		cVector2l m_vSize;
		cVector2l m_vGridNum;
		cVector2l m_vGridSize;
		cVector2l m_vMaxGridSpan;
	};
};
#endif