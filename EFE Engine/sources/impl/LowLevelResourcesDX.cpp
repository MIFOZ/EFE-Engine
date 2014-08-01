#include "impl/LowLevelResourcesDX.h"
#include "impl/MeshLoaderCollada.h"
#include "impl/VideoStreamTheora.h"

#include "impl/DX11Bitmap2D.h"
#include "impl/Platform.h"
#include "system/String.h"
#include "resources/MeshLoaderHandler.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cLowLevelResourcesDX::cLowLevelResourcesDX(cLowLevelGraphicsDX11 *a_pLowLevelGraphics)
	{
		m_vImageFormats[0] = "BMP";m_vImageFormats[1] = "LBM";m_vImageFormats[2] = "PCX";
		m_vImageFormats[3] = "GIF";m_vImageFormats[4] = "JPEG";m_vImageFormats[5] = "PNG";
		m_vImageFormats[6] = "JPG";
		m_vImageFormats[7] = "TGA";m_vImageFormats[8] = "TIFF";m_vImageFormats[9] = "TIF";
		m_vImageFormats[10] = "";

		m_pLowLevelGraphics = a_pLowLevelGraphics;
	}

	cLowLevelResourcesDX::~cLowLevelResourcesDX()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iBitmap2D *cLowLevelResourcesDX::LoadBitmap2D(tString a_sFilePath, tString a_sType)
	{
		tString tType;
		if (a_sType != "")
			a_sFilePath = cString::SetFileExt(a_sFilePath,a_sType);

		tType = cString::GetFileExt(a_sFilePath);
		cDX11Bitmap2D *pBmp = efeNew(cDX11Bitmap2D, ());

		if (tType=="bmp")
		{
			if (pBmp->LoadBmp(a_sFilePath.c_str())==false)
			{
				efeDelete(pBmp);
				return NULL;
			}

		}

		pBmp->SetPath(a_sFilePath);

		//tString sSavePath = "Saved\\" + cString::GetFileName(a_sFilePath);
		//pBmp->SaveBmp(sSavePath);

		return pBmp;
	}

	void cLowLevelResourcesDX::GetSupportedImageFormats(tStringList &a_lstFormats)
	{
		int lPos=0;

		while(m_vImageFormats[lPos]!="")
		{
			a_lstFormats.push_back(m_vImageFormats[lPos]);
			lPos++;
		}
	}

	void cLowLevelResourcesDX::AddMeshLoaders(cMeshLoaderHandler *a_pHandler)
	{
		a_pHandler->AddLoader(efeNew(cMeshLoaderCollada,(m_pLowLevelGraphics)));
	}

	void cLowLevelResourcesDX::FindFilesInDirs(tWStringList &a_lstStrings, tWString a_sDir, tWString a_sMask)
	{
		Platform::FindFileInDirs(a_lstStrings, a_sDir, a_sMask);
	}
}