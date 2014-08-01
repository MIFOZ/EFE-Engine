#ifndef EFE_ANIMATION_H
#define EFE_ANIMATION_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "resources/ResourceBase.h"

namespace efe
{
	class cAnimationTrack;

	typedef std::vector<cAnimationTrack*> tAnimationTrackVec;
	typedef tAnimationTrackVec::iterator tAnimationTrackVecIt;

	class cAnimation : public iResourceBase
	{
	public:
		cAnimation(const tString &a_sName, const tString &a_sFile);
		~cAnimation();

		float GetLength();
		void SetLength(float a_fTime);

		cAnimationTrack *CreateTrack(const tString &a_sName, tAnimTransformFlag a_Flags);
		cAnimationTrack *GetTrack(int a_lIndex);
		cAnimationTrack *GetTrackByName(const tString &a_sName);
		void ResizeTracks(int a_lNum);
		int GetTrackNum();

		const char *GetAnimationName(){return m_sFilePath.c_str();}
		void SetAnimationName(const tString &a_sName) {m_sAnimName = a_sName;}

		tString &GetFileName() {return m_sFileName;}

		//Resources implementation
		bool Reload() {return false;}
		void Unload(){}
		void Destroy(){}

	private:
		tString m_sAnimName;
		tString m_sFileName;

		float m_fLength;

		tAnimationTrackVec m_vTracks;
	};

};

#endif