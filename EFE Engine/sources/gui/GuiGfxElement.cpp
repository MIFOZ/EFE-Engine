#include "gui/GuiGfxElement.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/ImageManager.h"
#include "resources/ResourceImage.h"
#include "resources/FrameBitmap.h"

#include "gui/Gui.h"

namespace efe
{
	void cGuiGfxAnimation::AddFrame(int a_lNum)
	{
		m_vFrames.push_back(a_lNum);
	}

	void cGuiGfxAnimation::SetType(eGuiGfxAnimationType a_Type)
	{
		m_Type = a_Type;
	}

	void cGuiGfxAnimation::SetFrameLength(float a_fLength)
	{
		m_fFrameLength = a_fLength;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cGuiGfxElement::cGuiGfxElement(cGui *a_pGui)
	{
		m_pGui = a_pGui;

		m_vVtx.resize(4);

		for (int i=0;i<4;i++)
		{
			m_vVtx[i].pos = 0;
			m_vVtx[i].tex = 0;
			m_vVtx[i].col = cColor(1,1);
		}

		//Position
		m_vVtx[0].pos.y = 1;
		m_vVtx[2].pos.x = 1;
		m_vVtx[3].pos.x = 1;
		m_vVtx[3].pos.y = 1;

		//Texture coordinate
		m_vVtx[1].tex.x = 1;
		m_vVtx[2].tex.x = 1;
		m_vVtx[2].tex.y = 1;
		m_vVtx[3].tex.y = 1;

		m_vImageSize = 0;

		m_vOffset = 0;
		m_vActiveSize = 0;

		for (int i=0;i<kMaxGuiTextures;++i)
		{
			m_vTextures[i] = NULL;
			m_vImages[i] = NULL;
		}

		m_lTextureNum = 0;
		m_lCurrentAnimation = 0;
		m_fCurrentFrame = 0;
		m_bForwardAnim = true;
		m_lActiveImage = 0;
		m_bAnimationPaused = false;

		m_bFlushed = false;
	}

	cGuiGfxElement::~cGuiGfxElement()
	{
		STLDeleteAll(m_vAnimations);

		if (m_vImageBufferVec.size()>0)
		{
			for (int i=0;i<(int)m_vImageBufferVec.size();i++)
			{}
		}
		else
		{
			for (int i=0;i<m_lTextureNum;i++)
			{
				if (m_vImages[i])
				{
				}
				else if (m_vTextures[i])
					m_pGui->GetResources()->GetTextureManager()->Destroy(m_vTextures[i]);
			}
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cGuiGfxElement::Update(float a_fTimeStep)
	{
		if (m_vAnimations.empty() || m_bAnimationPaused) return;
		if (m_lCurrentAnimation >= (int)m_vAnimations.size()) return;

		cGuiGfxAnimation *pAnim = m_vAnimations[m_lCurrentAnimation];

		int lFrame = 0;

		// Non random anim update
		if (pAnim->m_Type != eGuiGfxAnimationType_Random)
		{
			m_fCurrentFrame += a_fTimeStep * (1.0f/pAnim->m_fFrameLength);
			lFrame = (int)m_fCurrentFrame;
			if (lFrame >= (int)m_vImageBufferVec.size())
			{
				if (pAnim->m_Type == eGuiGfxAnimationType_StopAtEnd)
				{
					lFrame = (int)m_vImageBufferVec.size()-1;
					m_fCurrentFrame = (float)lFrame;
				}
				else if(pAnim->m_Type == eGuiGfxAnimationType_Loop)
				{
					lFrame = 0;
					m_fCurrentFrame = 0;
				}
				else if(pAnim->m_Type == eGuiGfxAnimationType_Oscillate)
				{
					lFrame = 1;
					m_fCurrentFrame = 1;
					m_bForwardAnim = !m_bForwardAnim;
				}
			}

			if (m_bForwardAnim == false && pAnim->m_Type == eGuiGfxAnimationType_Oscillate)
				lFrame = ((int)m_vImageBufferVec.size()-1) - lFrame;
		}
		else if(m_vImageBufferVec.size() > 1)
		{
			float fPrev = m_fCurrentFrame;
			m_fCurrentFrame += a_fTimeStep * (1.0f/pAnim->m_fFrameLength);
			lFrame = (int)m_fCurrentFrame;
			if ((int)m_fCurrentFrame != (int)fPrev)
			{
				int lPrev = (int)fPrev;
				do
				{
					lFrame = cMath::RandRectl(0, (int)m_vImageBufferVec.size()-1);
				}
				while (lFrame == lPrev);

				m_fCurrentFrame = (float)lFrame;
			}
		}
		//Anim stuff

		if (lFrame != m_lActiveImage)
		{
			m_lActiveImage = lFrame;
			SetImage(m_vImageBufferVec[m_lActiveImage],0);
		}
	}

	//--------------------------------------------------------------

	void cGuiGfxElement::AddImage(cResourceImage *a_pImage)
	{
		SetImage(a_pImage, m_lTextureNum);

		m_vActiveSize = GetImageSize();

		++m_lTextureNum;
	}

	//--------------------------------------------------------------

	void cGuiGfxElement::AddTexture(iTexture *a_pTexture)
	{
		m_vTextures[m_lTextureNum] = a_pTexture;

		if (m_lTextureNum==0)
		{
			m_vImageSize.x = (float)a_pTexture->GetWidth();
			m_vImageSize.y = (float)a_pTexture->GetHeight();
		}

		m_vActiveSize = GetImageSize();

		++m_lTextureNum;
	}

	//--------------------------------------------------------------

	void cGuiGfxElement::AddImageToBuffer(cResourceImage *a_pImage)
	{
		if (m_vImageBufferVec.size()==0)
			SetImage(a_pImage,0);

		m_vImageBufferVec.push_back(a_pImage);
	}

	//--------------------------------------------------------------

	cGuiGfxAnimation *cGuiGfxElement::CreateAnimation(const tString &a_sName)
	{
		cGuiGfxAnimation *pAnimation = efeNew(cGuiGfxAnimation, ());
		pAnimation->m_sName = a_sName;

		m_vAnimations.push_back(pAnimation);

		return pAnimation;
	}

	//--------------------------------------------------------------

	void cGuiGfxElement::SetAnimationTime(float a_fTime)
	{
		if (m_lCurrentAnimation >= 0)
			m_fCurrentFrame = a_fTime / m_vAnimations[m_lCurrentAnimation]->m_fFrameLength;
		else
			m_fCurrentFrame = a_fTime;

	}

	//--------------------------------------------------------------

	void cGuiGfxElement::SetMaterial(iGuiMaterial *a_pMat)
	{
		m_pMaterial = a_pMat;
	}

	//--------------------------------------------------------------

	void cGuiGfxElement::SetColor(const cColor &a_Color)
	{
		for (int i=0;i<4;i++) m_vVtx[i].col = a_Color;
	}

	//--------------------------------------------------------------

	cVector2f cGuiGfxElement::GetImageSize()
	{
		return m_vImageSize;
	}

	//--------------------------------------------------------------

	void cGuiGfxElement::Flush()
	{
		if (m_bFlushed) return;

		for (int i=0;i<m_lTextureNum;++i)
			if (m_vImages[i]) m_vImages[i]->GetFrameBitmap()->FlushToTexture();
		for (size_t i=0;i<m_vImageBufferVec.size();++i)
			if (m_vImageBufferVec[i]) m_vImageBufferVec[i]->GetFrameBitmap()->FlushToTexture();

		m_bFlushed = true;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cGuiGfxElement::SetImage(cResourceImage *a_pImage, int a_lNum)
	{
		m_vImages[a_lNum] = a_pImage;
		m_vTextures[a_lNum] = a_pImage->GetTexture();

		const tVertexVec &vImageVtx = a_pImage->GetVertexVec();
		for (int i=0; i<4; ++i) m_vVtx[i].tex = vImageVtx[i].tex;

		if (a_lNum==0)
		{
			m_vImageSize.x = (float)a_pImage->GetWidth();
			m_vImageSize.y = (float)a_pImage->GetHeight();
		}
	}
}