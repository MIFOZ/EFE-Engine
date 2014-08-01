#ifndef EFE_MESH2D_H
#define EFE_MESH2D_H

#include <vector>
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

namespace efe
{
	class cMesh2DEdge
	{
	public:
		cMesh2DEdge(cVector2f a_vNormal, cVector2f a_vMidPos, unsigned int a_lStartIndex, unsigned int a_lEndIndex)
			: m_vNormal(a_vNormal), m_vMidPos(a_vMidPos), m_lStartIndex(a_lStartIndex), m_lEndIndex(a_lEndIndex) {}

		cVector2f m_vNormal;
		cVector2f m_vMidPos;
		unsigned int m_lStartIndex;
		unsigned int m_lEndIndex;
	};

	typedef std::vector<cMesh2DEdge> tMesh2DEdgeVec;
	typedef tMesh2DEdgeVec::iterator tMesh2DEdgeVecIt;

	class cCollisionMesh2D
	{
	public:
		cCollisionMesh2D(){}

		tVector2fVec m_vPos;
		tVector2fVec m_vNormal;
	};

	class cMesh2D
	{
	public:
		cMesh2D();
		~cMesh2D();

		void AddVertex(cVector2f a_vPos, cVector2f a_vTexCoord = 0, cColor a_Col = 1);
		void AddIndex(unsigned int a_lIndex);
		void AddEdgeIndex(unsigned int a_lIndex);

		void CreateVertexVec();
		void CreateTileVertexVec();

		tVertexVec *GetVertexVec(const cRectf &a_ImageRect, cVector2f a_vSize, eTileRotation a_Rotation=eTileRotation_0);
		tUIntVec *GetIndexVec();
		tMesh2DEdgeVec *GetEdgeVec(eTileRotation a_Rotation=eTileRotation_0);

		bool PointIsInside(const cVector2f &a_vPoint, const cVector2f &a_vMeshPos,
							eTileRotation a_Rotation=eTileRotation_0);

		cCollisionMesh2D *CreateCollisionMesh(const cVector2f &a_vPos, const cVector2f &a_vSizeMul,
										eTileRotation a_Rotation=eTileRotation_0);

	private:
		tVector2fVec m_vPos;
		tColorVec m_vColor;
		tMesh2DEdgeVec m_vEdge[eTileRotation_LastEnum];
		tVector2fVec m_vTexCoord;
		tUIntVec m_vIndex;
		tUIntVec m_vEdgeIndex;
		cRectf m_vBoundingBox[eTileRotation_LastEnum];

		tVertexVec m_vVtx[eTileRotation_LastEnum];

		void CalculateEdges(eTileRotation a_Rotation,tVertexVec &a_Vtx, tUIntVec &a_Idx);
	};
};
#endif