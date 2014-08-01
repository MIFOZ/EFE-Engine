#include "graphics/BackgroundImage.h"
#include "math/Math.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cBackgroundImage::cBackgroundImage(iMaterial *a_pMat, const cVector3f &a_vPos, bool a_bTile,
				const cVector2f &a_vSize, const cVector2f &a_vPosPercent, const cVector2f &a_vVel)
	{
		m_pMaterial = a_pMat;

		m_vPos = a_vPos;
		m_bTile = a_bTile;
		m_vSize = a_vSize;
		m_vPosPercent = a_vPosPercent;
		m_vVel = a_vVel;

		m_vVtx = a_pMat->GetImage(eMaterialTexture_Diffuse)->GetVertexVecCopy(0, m_vSize);
		for (int i=0;i<(int)m_vVtx.size();i++)
			m_vVtx[i].pos.z = m_vPos.z;
	}

	//--------------------------------------------------------------

	cBackgroundImage::~cBackgroundImage()
	{
		efeDelete(m_pMaterial);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cBackgroundImage::Draw(const cRectf &a_CollideRect, iLowLevelGraphics *a_pLowLevelGraphics)
	{
		tVector3fList lstPositions;
		cVector3f vScreenPos(a_CollideRect.x * m_vPosPercent.x - m_vPos.x,
			a_CollideRect.y * m_vPosPercent.y, m_vPos.z - m_vPos.y);

		if (m_bTile)
		{
			cVector2l vNum;
			vNum.x = (int)(a_CollideRect.w / m_vSize.x) + 1;
			vNum.y = (int)(a_CollideRect.h / m_vSize.y) + 1;

			cVector2f vStartPos;
			if (vScreenPos.x>=0)
				vStartPos.x = m_vSize.x - cMath::Modulus(vScreenPos.x, m_vSize.x);
			else
				vStartPos.x = cMath::Modulus(vScreenPos.x, m_vSize.x);

			if (vScreenPos.y>=0)
				vStartPos.y = m_vSize.y - cMath::Modulus(vScreenPos.y, m_vSize.y);
			else
				vStartPos.y = cMath::Modulus(vScreenPos.y, m_vSize.y);

			if (vStartPos.x > 0)
			{
				vStartPos.x -= m_vSize.x;
				vNum.x++;
			}
			if (vStartPos.y > 0)
			{
				vStartPos.y -= m_vSize.y;
				vNum.y++;
			}

			for (int x=0; x<vNum.x; x++)
			{
				for (int y=0; y<vNum.y; y++)
					lstPositions.push_back(cVector3f(vStartPos.x + m_vSize.x * x,
											vStartPos.y + m_vSize.y * y, vScreenPos.z));
			}
		}
		else
		{
			cRectf Rect(vScreenPos.x, vScreenPos.y, m_vSize.x, m_vSize.y);

			if (cMath::BoxCollision(a_CollideRect, Rect))
				lstPositions.push_back(vScreenPos);
		}

		m_pMaterial->StartRendering(eMaterialRenderType_Perspective, NULL, NULL);

		int IdxAdd = 0;

		tVector3fListIt it = lstPositions.begin();
		for (;it!=lstPositions.end();it++)
		{
			m_vVtx[0].pos.x = it->x;
			m_vVtx[0].pos.y = it->y;
			a_pLowLevelGraphics->AddVertexToBatch(&m_vVtx[0]);

			m_vVtx[1].pos.x = it->x + m_vSize.x;
			m_vVtx[1].pos.y = it->y;
			a_pLowLevelGraphics->AddVertexToBatch(&m_vVtx[1]);

			m_vVtx[2].pos.x = it->x + m_vSize.x;
			m_vVtx[2].pos.y = it->y + m_vSize.y;
			a_pLowLevelGraphics->AddVertexToBatch(&m_vVtx[2]);

			m_vVtx[3].pos.x = it->x;
			m_vVtx[3].pos.y = it->y + m_vSize.y;
			a_pLowLevelGraphics->AddVertexToBatch(&m_vVtx[3]);

			a_pLowLevelGraphics->AddIndexToBatch(IdxAdd+0);
			a_pLowLevelGraphics->AddIndexToBatch(IdxAdd+1);
			a_pLowLevelGraphics->AddIndexToBatch(IdxAdd+2);
			a_pLowLevelGraphics->AddIndexToBatch(IdxAdd+3);

			IdxAdd+=4;
		}

		/*do
		{*/
			a_pLowLevelGraphics->FlushQuadBatch(false);
		/*} 
		while(m_pMaterial-*/
		a_pLowLevelGraphics->ClearBatch();

		//m_pMaterial-
	}

	void cBackgroundImage::Update()
	{
		m_vPos += m_vVel;

		if (m_bTile)
		{
			if (m_vPos.x>=m_vSize.x) m_vPos.x = m_vPos.x - m_vSize.x;
			if (m_vPos.y>=m_vSize.x) m_vPos.y = m_vPos.y - m_vSize.y;
		}
	}
}