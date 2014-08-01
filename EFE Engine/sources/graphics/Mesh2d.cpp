#include "graphics/Mesh2d.h"
#include <math.h>
#include "math/MathTypes.h"
#include "math/Math.h"
#include "system/MemoryManager.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMesh2D::cMesh2D()
	{
	}

	cMesh2D::~cMesh2D()
	{
		m_vPos.clear();
		m_vTexCoord.clear();
		m_vColor.clear();
		m_vIndex.clear();

		for (int i=0;i<eTileRotation_LastEnum;i++)
			m_vVtx[i].clear();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cMesh2D::AddVertex(cVector2f a_vPos, cVector2f a_vTexCoord, cColor a_Col)
	{
		m_vPos.push_back(a_vPos);
		m_vTexCoord.push_back(a_vTexCoord);
		m_vColor.push_back(a_Col);
	}

	//--------------------------------------------------------------

	void cMesh2D::AddIndex(unsigned int a_lIndex)
	{
		m_vIndex.push_back(a_lIndex);
	}

	//--------------------------------------------------------------

	void cMesh2D::AddEdgeIndex(unsigned int a_lIndex)
	{
		m_vEdgeIndex.push_back(a_lIndex);
	}

	//--------------------------------------------------------------

	void cMesh2D::CreateVertexVec()
	{
		for (int i=0;i<(int)m_vPos.size();i++)
			m_vVtx[0].push_back(cVertex(cVector3f(m_vPos[i].x,m_vPos[i].y,0),cVector3f(m_vTexCoord[i].x,m_vTexCoord[i].y,0),m_vColor[i]));

		CalculateEdges(eTileRotation_0,m_vVtx[0],m_vEdgeIndex);
	}

	//--------------------------------------------------------------

	void cMesh2D::CreateTileVertexVec()
	{
		int i;
		CreateVertexVec();

		for (i=1;i<eTileRotation_LastEnum;i++)
			m_vVtx[i]=m_vVtx[0];

		for (int angle=1;angle<4;angle++)
		{
			for (i=0;i<(int)m_vVtx[0].size();i++)
			{
				float fAngle = ((float)angle)*kPi2f;
				m_vVtx[angle][i].pos.x =	cos(fAngle)*m_vVtx[0][i].pos.x -
											sin(fAngle)*m_vVtx[0][i].pos.y;

				m_vVtx[angle][i].pos.y =	sin(fAngle)*m_vVtx[0][i].pos.x +
											cos(fAngle)*m_vVtx[0][i].pos.y;
			}

			CalculateEdges((eTileRotation)angle,m_vVtx[angle],m_vEdgeIndex);
		}
	}

	//--------------------------------------------------------------

	tVertexVec *cMesh2D::GetVertexVec(const cRectf &a_ImageRect, cVector2f a_vSize, eTileRotation a_Rotation)
	{
		cVector3f vImageStart(a_ImageRect.x, a_ImageRect.y, 0);
		cVector3f vImageSize(a_ImageRect.w, a_ImageRect.h, 0);

		for (int j=0;j<(int)m_vVtx[a_Rotation].size();j++)
		{
			cVector3f vPos = (m_vVtx[0][j].pos + a_vSize/2) / a_vSize;

			m_vVtx[a_Rotation][j].col = 1;
			m_vVtx[a_Rotation][j].tex = vImageStart + (vPos*vImageSize);
			switch(a_Rotation)
			{
			case eTileRotation_0: m_vVtx[a_Rotation][j].norm = cVector3f(1,0,3);break;
			case eTileRotation_90: m_vVtx[a_Rotation][j].norm = cVector3f(0,1,3);break;
			case eTileRotation_180: m_vVtx[a_Rotation][j].norm = cVector3f(-1,0,3);break;
			case eTileRotation_270: m_vVtx[a_Rotation][j].norm = cVector3f(0,-1,3);break;
			}
		}

		return &m_vVtx[a_Rotation];
	}

	//--------------------------------------------------------------

	tUIntVec *cMesh2D::GetIndexVec()
	{
		return &m_vIndex;
	}

	//--------------------------------------------------------------

	tMesh2DEdgeVec *cMesh2D::GetEdgeVec(eTileRotation a_Rotation)
	{
		return &m_vEdge[a_Rotation];
	}

	//--------------------------------------------------------------

	bool cMesh2D::PointIsInside(const cVector2f &a_vPoint, const cVector2f &a_vMeshPos,
							eTileRotation a_Rotation)
	{
		cVector2f vLocalPoint;
		cVector2f vNormal;

		for (int i=0;i<(int)m_vEdge[a_Rotation].size();i++)
		{
			vLocalPoint = a_vPoint - a_vMeshPos - m_vEdge[a_Rotation][i].m_vMidPos;
			vNormal = m_vEdge[a_Rotation][i].m_vNormal;

			if ((vLocalPoint.x* vNormal.x + vLocalPoint.y*vNormal.y)>=0) return false;
		}
		return true;
	}

	//--------------------------------------------------------------

	cCollisionMesh2D *cMesh2D::CreateCollisionMesh(const cVector2f &a_vPos, const cVector2f &a_vSizeMul,
										eTileRotation a_Rotation)
	{
		cCollisionMesh2D *pCollMesh = efeNew(cCollisionMesh2D, ());

		for (int i=0;i<(int)m_vIndex.size();i++)
		{
			cVector3f vPos = m_vVtx[a_Rotation][m_vEdgeIndex[i]].pos;
			//pCollMesh->m_vPos.push_back(D3DXVECTOR2(vPos.x, vPos.y)
		}

		for (int i=0;i<(int)m_vEdge[a_Rotation].size();i++)
			pCollMesh->m_vNormal.push_back(m_vEdge[a_Rotation][i].m_vNormal);

		return pCollMesh;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cMesh2D::CalculateEdges(eTileRotation a_Rotation,tVertexVec &a_Vtx, tUIntVec &a_Idx)
	{
		
	}
}