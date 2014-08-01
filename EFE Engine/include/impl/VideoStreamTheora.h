#ifndef EFE_VIDEO_STREAM_THEORA_H
#define EFE_VIDEO_STREAM_THEORA_H

#include "graphics/VideoStream.h"

#include <theora/theora.h>

namespace efe
{
	class cVideoStreamTheora_Loader;

	class cVideoStreamTheora : public iVideoStream
	{
	public:
		cVideoStreamTheora(tString a_sName, cVideoStreamTheora_Loader *a_pLoader);
		~cVideoStreamTheora();

		bool LoadFromFile(tString a_sFilePath);

		void Update(float a_fTimeStep);

		void Play();
		void Stop();

		void Pause(bool a_bX);
		bool IsPaused(){return m_bPaused;}

		void SetLoop(bool a_bX);
		bool IsLooping(){return m_bLooping;}

		void CopyToTexture(iTexture *a_pTexture);

	private:
		void DrawFrameToBuffer();
		int BufferData(FILE *a_pFile, ogg_sync_state *a_pOggSyncState);
		void QueuePage(ogg_page *a_pPage);
		bool GetHeaders();
		bool InitDecoders();
		void ResetStreams();

		cVideoStreamTheora_Loader *m_pLoader;

		FILE *m_pFile;

		bool m_bLooping;
		bool m_bPaused;
		bool m_bPlaying;

		float m_fTime;

		unsigned char *m_pFrameBuffer;

		ogg_sync_state m_OggSyncState;
		ogg_stream_state m_TheoraStreamState;
		theora_info m_TheoraInfo;
		theora_comment m_TheoraComment;
		theora_state m_TheoraState;

		int m_lVideobufReady;
		ogg_int64_t m_lVideobufGranulePos;
		double m_fVideobufTime;

		bool m_bVideoLoaded;
		bool m_bVideoFrameReady;
		int m_lBufferSize;
	};

	class cVideoStreamTheora_Loader : public iVideoStreamLoader
	{
		friend class cVideoStreamTheora;
	public:
		cVideoStreamTheora_Loader();
		~cVideoStreamTheora_Loader();

		iVideoStream *Create(const tString &a_sName){return efeNew(cVideoStreamTheora, (a_sName, this));}

	private:
		unsigned char *m_pYuvToR;
		unsigned char *m_pYuvToB;

		unsigned short *m_pYuv_G_UV;
		unsigned char *m_pYuv_G_Y_UV;
	};
};
#endif