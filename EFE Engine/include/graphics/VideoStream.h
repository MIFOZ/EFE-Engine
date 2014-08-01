#ifndef EFE_VIDEO_STREAM_H
#define EFE_VIDEO_STREAM_H

#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace efe
{
	class iTexture;

	class iVideoStream;

	class iVideoStreamLoader
	{
	public:
		virtual ~iVideoStreamLoader(){}

		virtual iVideoStream *Create(const tString &a_sName)=0;

		tStringVec &GetExtensions(){return m_vExtensions;}
	protected:
		tStringVec m_vExtensions;
	};

	class iVideoStream : public iResourceBase
	{
	public:
		iVideoStream(tString a_sName) : iResourceBase(a_sName,0){}
		virtual ~iVideoStream(){}

		virtual bool LoadFromFile(tString a_sFilePath)=0;

		virtual void Update(float a_fTimeStep)=0;

		virtual void Play()=0;
		virtual void Stop()=0;

		virtual void Pause(bool a_bX)=0;
		virtual bool IsPaused()=0;

		virtual void SetLoop(bool a_bX)=0;
		virtual bool IsLooping()=0;

		virtual void CopyToTexture(iTexture *a_pTexture)=0;

		const tString &GetFileName(){return m_sFilePath;}
		const cVector2l &GetSize(){return m_vSize;}

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}
	protected:
		tString m_sFilePath;
		cVector2l m_vSize;
	};
};

#endif
