#ifndef EFE_GUI_GFX_ELEMENT_H
#define EFE_GUI_GFX_ELEMENT_H

#include "gui/GuiTypes.h"

namespace efe
{
	class iGuiMaterial;
	class iTexture;
	class cResourceImage;

	class cGui;

	#define kMaxGuiTextures (4)

	enum eGuiGfxAnimationType
	{
		eGuiGfxAnimationType_Loop,
		eGuiGfxAnimationType_StopAtEnd,
		eGuiGfxAnimationType_Oscillate,
		eGuiGfxAnimationType_Random,
		eGuiGfxAnimationType_LastEnum
	};

	class cGuiGfxAnimation
	{
		friend class cGuiGfxElement;
	public:
		cGuiGfxAnimation() : m_fFrameLength(1), m_Type(eGuiGfxAnimationType_Loop){}

		void AddFrame(int a_lNum);
		void SetType(eGuiGfxAnimationType a_Type);
		void SetFrameLength(float a_fLength);
	private:
		tString m_sName;
		std::vector<int> m_vFrames;
		float m_fFrameLength;
		eGuiGfxAnimationType m_Type;
	};

	class cGuiGfxElement
	{
		friend class cGuiSet;
		friend class cGuiRenderObjectCompare;
	public:
		cGuiGfxElement(cGui *a_pGui);
		~cGuiGfxElement();

		void Update(float a_fTimeStep);

		void AddImage(cResourceImage *a_pImage);
		void AddTexture(iTexture *a_pTexture);

		void AddImageToBuffer(cResourceImage *a_pImage);

		void SetOffset(const cVector3f &a_vOffset){m_vOffset = a_vOffset;}
		const cVector3f &GetOffset(){return m_vOffset;}

		void SetActiveSize(const cVector2f &a_vSize){m_vActiveSize = a_vSize;}
		const cVector2f &GetActiveSize(){return m_vActiveSize;}

		cGuiGfxAnimation *CreateAnimation(const tString &a_sName);

		cGuiGfxAnimation *GetAnimation(int a_lIdx){return m_vAnimations[a_lIdx];}
		void SetAnimationTime(float a_fTime);

		void SetMaterial(iGuiMaterial *a_pMat);

		void SetColor(const cColor &a_Color);

		cVector2f GetImageSize();

		void Flush();
	private:
		void SetImage(cResourceImage *a_pImage, int a_lNum);

		cVector2f m_vImageSize;
		cGui *m_pGui;
		tVertexVec m_vVtx;

		cVector3f m_vOffset;
		cVector2f m_vActiveSize;

		iGuiMaterial *m_pMaterial;
		iTexture *m_vTextures[kMaxGuiTextures];
		cResourceImage *m_vImages[kMaxGuiTextures];

		std::vector<cResourceImage*> m_vImageBufferVec;

		std::vector<cGuiGfxAnimation*> m_vAnimations;
		int m_lCurrentAnimation;
		float m_fCurrentFrame;
		int m_lActiveImage;
		bool m_bForwardAnim;
		bool m_bAnimationPaused;

		int m_lTextureNum;

		bool m_bFlushed;
	};
};
#endif