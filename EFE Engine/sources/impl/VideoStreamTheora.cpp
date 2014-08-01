#include "impl/VideoStreamTheora.h"

#include "system/LowLevelSystem.h"
#include "math/Math.h"
#include "graphics/Texture.h"

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "libogg.lib")
#pragma comment(lib, "libtheora.lib")

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	cVideoStreamTheora_Loader::cVideoStreamTheora_Loader()
	{
		m_vExtensions.push_back("ogm");
		m_vExtensions.push_back("ogg");

		m_pYuvToR = efeNewArray(unsigned char, 256*256);
		m_pYuvToB = efeNewArray(unsigned char, 256*256);

		m_pYuv_G_UV = efeNewArray(unsigned short, 256*256);
		m_pYuv_G_Y_UV = efeNewArray(unsigned char, 256*1024);

		float fY, fI, fJ, fUV;
		for (int i=0;i<256;++i)
		{
			for (int y=0;y<256;++y)
			{
				fY = 1.164f * ((float)y - 16);
				fI = (float)i;

				m_pYuvToR[y + i*256] = (unsigned char)cMath::Clamp(fY + 1.596f * (fI - 128),0,255);

				m_pYuvToB[y + i*256] = (unsigned char)cMath::Clamp(fY + 2.018f * (fI - 128),0,255);
			}
		}

		float fMinVal = 0.813f * (-128.0f) - 0.391f * 127;
		float fMaxVal = 0.813f * (128.0f) + 0.391f * 127;

		for (int i=0;i<256;++i)
		{
			for (int j=0;j<256;++j)
			{
				fJ = (float)j;
				fI = (float)i;

				float fUV = 0.813f * (fJ - 128) - 0.391f * (fI - 128);
				m_pYuv_G_UV[i*256 + j] = (unsigned char)cMath::Clamp(((fUV - fMinVal) / (fMaxVal - fMinVal)) * 1023.0f,0,1023.0f);
				
			}
		}

		for (int i=0;i<1024;++i)
		{
			for (int y=0;y<256;++y)
			{
				fY = 1.164f * ((float)y - 16);
				fUV = (float)i;
				fUV = (fUV / 1023.0f) * (fMaxVal - fMinVal) + fMinVal;

				m_pYuv_G_Y_UV[i*256 + y] = (unsigned short)cMath::Clamp(fY - fUV,0,255);
			}
		}
	}

	cVideoStreamTheora_Loader::~cVideoStreamTheora_Loader()
	{
		efeDeleteArray(m_pYuvToR);
		efeDeleteArray(m_pYuvToB);

		efeDeleteArray(m_pYuv_G_UV);
		efeDeleteArray(m_pYuv_G_Y_UV);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cVideoStreamTheora::cVideoStreamTheora(tString a_sName, cVideoStreamTheora_Loader *a_pLoader) : iVideoStream(a_sName)
	{
		m_pLoader = a_pLoader;

		m_pFile = NULL;

		m_bLooping = false;
		m_bPaused = false;
		m_bPlaying = false;

		m_lVideobufReady = 0;
		m_lVideobufGranulePos = -1;
		m_fVideobufTime = 0;

		m_fTime = 0;

		m_lBufferSize = 4096;

		m_bVideoFrameReady = false;

		m_pFrameBuffer = NULL;
			
		theora_comment_init(&m_TheoraComment);
		theora_info_init(&m_TheoraInfo);

		ogg_sync_init(&m_OggSyncState);
	}

	//--------------------------------------------------------------

	cVideoStreamTheora::~cVideoStreamTheora()
	{
		if (m_pFile) fclose(m_pFile);

		ogg_sync_clear(&m_OggSyncState);

		theora_comment_clear(&m_TheoraComment);
		theora_info_clear(&m_TheoraInfo);

		ogg_stream_clear(&m_TheoraStreamState);

		if (m_bVideoLoaded)
		{
			theora_clear(&m_TheoraState);
			if (m_pFrameBuffer) efeDeleteArray(m_pFrameBuffer);
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	bool cVideoStreamTheora::LoadFromFile(tString a_sFilePath)
	{
		m_pFile = fopen(a_sFilePath.c_str(),"rb");
		if (m_pFile == NULL)
			return false;

		m_sFilePath = a_sFilePath;

		m_bVideoLoaded = false;

		if (GetHeaders()==false) return false;

		if (InitDecoders()==false) return false;

		return true;
	}

	//-------------------------------------------------------------

	void cVideoStreamTheora::Update(float a_fTimeStep)
	{
		if (m_bPlaying==false || m_bPaused) return;

		m_fTime += a_fTimeStep;

		while (!m_lVideobufReady && m_fVideobufTime < m_fTime)
		{
			ogg_packet packet;
			if (ogg_stream_packetout(&m_TheoraStreamState, &packet) > 0)
			{
				if (theora_decode_packetin(&m_TheoraState, &packet) == 0)
				{
					m_lVideobufGranulePos = m_TheoraState.granulepos;
					m_fVideobufTime = theora_granule_time(&m_TheoraState, m_lVideobufGranulePos);

					m_bVideoFrameReady = true;
				}
			}
			else
			{
				ogg_page page;
				if (ogg_sync_pageout(&m_OggSyncState, &page) > 0)
				{
					QueuePage(&page);
				}
				//No pages, read more buffer data
				else
				{
					int bytes = BufferData(m_pFile, &m_OggSyncState);

					if (bytes != 0)
					{
						while(ogg_sync_pageout(&m_OggSyncState, &page) > 0)
							QueuePage(&page);
					}
					else
					{
						if (m_bLooping==false) m_bPlaying = false;
						ResetStreams();
					}
				}
			}
		}

		if (!m_bVideoFrameReady && feof(m_pFile))
		{
			if (m_bLooping==false) m_bPlaying = false;
			ResetStreams();
		}
	}

	//-------------------------------------------------------------

	void cVideoStreamTheora::Play()
	{
		m_bPlaying = true;
	}

	//-------------------------------------------------------------

	void cVideoStreamTheora::Stop()
	{
		m_bPlaying = false;
		ResetStreams();
	}

	//------------------------------------------------------------

	void cVideoStreamTheora::Pause(bool a_bX)
	{
		m_bPaused = a_bX;
	}

	//------------------------------------------------------------

	void cVideoStreamTheora::SetLoop(bool a_bX)
	{
		m_bLooping = a_bX;
	}

	//------------------------------------------------------------

	void cVideoStreamTheora::CopyToTexture(iTexture *a_pTexture)
	{
		if (m_bVideoLoaded==false || m_pFrameBuffer == NULL) return;

		if (m_bVideoFrameReady)
		{
			DrawFrameToBuffer();

			a_pTexture->SetPixels2D(0, 0, m_vSize, eFormat_RGB8, m_pFrameBuffer);

			m_bVideoFrameReady = false;
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	void cVideoStreamTheora::DrawFrameToBuffer()
	{
		yuv_buffer yuvBuffer;
		int qwe = theora_decode_YUVout(&m_TheoraState, &yuvBuffer);

		if (yuvBuffer.uv_stride > 1000)
			return;

		size_t lCropOffsetY = m_TheoraInfo.offset_x + yuvBuffer.y_stride * m_TheoraInfo.offset_y;
		size_t lCropOffsetUV = m_TheoraInfo.offset_x/2 + yuvBuffer.uv_stride * (m_TheoraInfo.offset_y/2);

		unsigned char *pYuvToR = m_pLoader->m_pYuvToR;
		unsigned char *pYuvToB = m_pLoader->m_pYuvToB;

		unsigned short *pYuv_G_UV = m_pLoader->m_pYuv_G_UV;
		unsigned char *pYuv_G_Y_UV = m_pLoader->m_pYuv_G_Y_UV;

		unsigned char *pSrcBuffer = m_pFrameBuffer;
		unsigned char *pYBuffer = yuvBuffer.y + lCropOffsetY;

		int lXCount = m_vSize.x;
		int lYCount = m_vSize.y;

		const int lYBufferAdd = yuvBuffer.y_stride - m_vSize.x;

		while(lYCount)
		{
			*pSrcBuffer = *pYBuffer;

			pSrcBuffer+=3;
			pYBuffer++;

			--lXCount;
			if (!lXCount)
			{
				pYBuffer += lYBufferAdd;
				lXCount = m_vSize.x;
				--lYCount;
			}
		}

		pSrcBuffer = m_pFrameBuffer;
		unsigned char *pUBuffer = yuvBuffer.u + lCropOffsetUV;
		unsigned char *pVBuffer = yuvBuffer.v + lCropOffsetUV;

		lXCount = m_vSize.x/2;
		lYCount = m_vSize.y/2;

		const int lXRowAdd = m_vSize.x/2;
		const int lSourceRowSize = m_vSize.x*3;
		const int lUVBufferAdd = yuvBuffer.uv_stride - m_vSize.x/2;
		unsigned char y;

		while(lYCount)
		{
			const unsigned short u_add = (*pUBuffer) << 8;
			const unsigned short v_add = (*pVBuffer) << 8;
			const unsigned int g_uv_add = (pYuv_G_UV[u_add + (*pVBuffer)])<<8;

			y = *pSrcBuffer;
			pSrcBuffer[0] = pYuvToR[y + v_add];
			pSrcBuffer[1] = pYuv_G_Y_UV[y + (g_uv_add)];
			pSrcBuffer[2] = pYuvToB[y + u_add];

			pSrcBuffer+=3;
			y = *pSrcBuffer;
			pSrcBuffer[0] = pYuvToR[y + v_add];
			pSrcBuffer[1] = pYuv_G_Y_UV[y + (g_uv_add)];
			pSrcBuffer[2] = pYuvToB[y + u_add];

			pSrcBuffer += lSourceRowSize - 3;
			y = *pSrcBuffer;
			pSrcBuffer[0] = pYuvToR[y + v_add];
			pSrcBuffer[1] = pYuv_G_Y_UV[y + (g_uv_add)];
			pSrcBuffer[2] = pYuvToB[y + u_add];

			pSrcBuffer+=3;
			y = *pSrcBuffer;
			pSrcBuffer[0] = pYuvToR[y + v_add];
			pSrcBuffer[1] = pYuv_G_Y_UV[y + (g_uv_add)];
			pSrcBuffer[2] = pYuvToB[y + u_add];

			pSrcBuffer -= lSourceRowSize;

			pSrcBuffer+=3;

			pUBuffer++;
			pVBuffer++;

			--lXCount;
			if (!lXCount)
			{
				pSrcBuffer += lSourceRowSize;
				pVBuffer += lUVBufferAdd;
				pUBuffer += lUVBufferAdd;
				lXCount = lXRowAdd;
				--lYCount;
			}
		}
	}

	//-------------------------------------------------------------

	int cVideoStreamTheora::BufferData(FILE *a_pFile, ogg_sync_state *a_pOggSyncState)
	{
		ogg_page tempPage;
		if (ogg_sync_pageout(a_pOggSyncState, &tempPage) == 1)
		{
			Warning("Still pages in buffer do not read another one! Go tell the programmer he messed up!\n");
			return 0;
		}

		char *pBuffer = ogg_sync_buffer(a_pOggSyncState, m_lBufferSize);
		int lBytes = (int)fread(pBuffer, 1, m_lBufferSize, a_pFile);

		ogg_sync_wrote(a_pOggSyncState, lBytes);

		return lBytes;
	}

	//-------------------------------------------------------------

	void cVideoStreamTheora::QueuePage(ogg_page *a_pPage)
	{
		if (m_bVideoLoaded)
		{
			if (ogg_stream_pagein(&m_TheoraStreamState, a_pPage)!=0)
			{
			}
		}
	}

	//-------------------------------------------------------------

	bool cVideoStreamTheora::GetHeaders()
	{
		int lTheoraHeaderPackets = 0;

		bool bFoundHeaders = false;
		while(!bFoundHeaders)
		{
			int lRet = BufferData(m_pFile, &m_OggSyncState);
			if (lRet == 0)
			{
				Error("Found no header in video file '%s'\n", m_sFilePath.c_str());
				return false;
			}

			ogg_page page;
			while (ogg_sync_pageout(&m_OggSyncState, &page)>0)
			{
				if (!ogg_page_bos(&page))
				{
					QueuePage(&page);

					continue;
				}

				ogg_stream_state testStream;
				ogg_stream_init(&testStream, ogg_page_serialno(&page));
				ogg_stream_pagein(&testStream, &page);

				ogg_packet testPacket;
				ogg_stream_packetout(&testStream, &testPacket);

				if (!m_bVideoLoaded && theora_decode_header(&m_TheoraInfo, &m_TheoraComment, &testPacket)>=0)
				{
					memcpy(&m_TheoraStreamState, &testStream, sizeof(testStream));
					lTheoraHeaderPackets = 1;
					m_bVideoLoaded = true;
				}
				else
					ogg_stream_clear(&testStream);

				if (m_bVideoLoaded) bFoundHeaders = true;
			}
		}

		while(lTheoraHeaderPackets && lTheoraHeaderPackets<3)
		{
			int lRet;
			ogg_packet packet;

			while(lTheoraHeaderPackets && lTheoraHeaderPackets<3)
			{
				lRet = ogg_stream_packetout(&m_TheoraStreamState, &packet);
				if (lRet == 0) break;

				if (lRet < 0)
				{
					Error("Error parsing Theora stream headers in '%s'; corrupt stream?\n", m_sFilePath.c_str());
					return false;
				}
				if (theora_decode_header(&m_TheoraInfo, &m_TheoraComment, &packet))
				{
					Error("Error parsing Theora stream headers in '%s'; corrupt stream?\n", m_sFilePath.c_str());
					return false;
				}
				lTheoraHeaderPackets++;
				if (lTheoraHeaderPackets==3)break;
			}

			ogg_page page;
			if (ogg_sync_pageout(&m_OggSyncState, &page)>0)
				QueuePage(&page);
			else
			{
				int lRet = BufferData(m_pFile, &m_OggSyncState);
				if (lRet==0)
				{
					Error("End of file while searching for codec headers in '%s'.\n", m_sFilePath.c_str());
					return false;
				}
			}
		}
		return true;
	}

	bool cVideoStreamTheora::InitDecoders()
	{
		if (m_bVideoLoaded)
		{
			theora_decode_init(&m_TheoraState, &m_TheoraInfo);

			m_vSize = cVector2l(m_TheoraInfo.frame_width, m_TheoraInfo.frame_height);

			m_pFrameBuffer = efeNewArray(unsigned char, m_vSize.x * m_vSize.y * 3);
		}
		else
		{
			theora_info_clear(&m_TheoraInfo);
			theora_comment_clear(&m_TheoraComment);
		}

		return true;
	}

	void cVideoStreamTheora::ResetStreams()
	{
		ogg_packet packet;
		while(ogg_stream_packetout(&m_TheoraStreamState, &packet)>0);

		ogg_page testPage;
		while(ogg_sync_pageout(&m_OggSyncState, &testPage));

		m_fTime = 0;
		m_fVideobufTime = 0;
		m_bVideoFrameReady = false;

		ogg_stream_clear(&m_TheoraStreamState);
		ogg_sync_reset(&m_OggSyncState);

		theora_clear(&m_TheoraState);
		theora_decode_init(&m_TheoraState, &m_TheoraInfo);

		fclose(m_pFile);
		m_pFile = fopen(m_sFilePath.c_str(), "rb");

		BufferData(m_pFile, &m_OggSyncState);

		int lPackCount = 0;
		while (lPackCount<3)
		{
			ogg_page page;
			while(lPackCount<3 && ogg_sync_pageout(&m_OggSyncState, &page)>0)
			{
				if (lPackCount == 0)
				{
					if (ogg_page_bos(&page))
					{
						ogg_stream_state testStream;
						ogg_packet testPacket;

						ogg_stream_init(&testStream, ogg_page_serialno(&page));
						ogg_stream_pagein(&testStream, &page);
						ogg_stream_packetout(&testStream, &testPacket);

						if (testPacket.packet[0] & 0x80)
						{
							memcpy(&m_TheoraStreamState, &testStream, sizeof(testStream));
							lPackCount = 1;
						}
						else
							ogg_stream_clear(&testStream);
					}
				}

				if (lPackCount > 0)
				{
					QueuePage(&page);

					while (lPackCount<3)
					{
						ogg_packet testPacket;
						int lRet = ogg_stream_packetout(&m_TheoraStreamState, &testPacket);
						if (lRet < 1) break;
						++lPackCount;
					}
				}
			}

			BufferData(m_pFile, &m_OggSyncState);
		}
	}
}