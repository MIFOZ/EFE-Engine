#ifndef EFE_PLATFORM_H
#define EFE_PLATFORM_H

#include "system/String.h"
#include <stdio.h>
#include <io.h>

namespace efe
{
	typedef unsigned char uint8;
	typedef char int8;

	typedef unsigned short uint16;
	typedef short int16;

	typedef unsigned int uint32;
	typedef int int32;

	typedef unsigned char ubyte;
	//typedef unsigned short ushort;
	/*typedef unsigned int uint;*/

#define MCHAR2(a, b) (a | (b << 8))

	class Platform
	{
	public:
		static long FileLength(const tWString &a_sFileName);
		static long FileLength(FILE *pFile);
		static void FindFileInDirs(tWStringList &a_lstStrings, tWString a_sDir, tWString a_sMask);
	};
};

#endif