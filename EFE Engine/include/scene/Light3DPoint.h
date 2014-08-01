#ifndef EFE_LIGHT3D_POINT_H
#define EFE_LIGHT3D_POINT_H

#include "scene/Light3D.h"

namespace efe
{
	//-------------------------------------

	/*kSaveData_ChildClass(iLight3D, cLight3DPoint)
	{
		kSaveData_ChildInit(cLight3DPoint)
	public:
		virtual iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);
		virtual int GetSaveCreatePrio();
	};*/

	class cLight3DPoint : public iLight3D
	{
	public:
		cLight3DPoint(tString a_sName, cResources *a_pResources);

		//SaveObject implementation
		/*virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);*/

	private:
		cSectorVisibilityContainer *CreateSectorVisibility();
		void UpdateBoundingVolume();
		bool CreateClipRect(cRectl &a_ClipRect, cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics);
	};
};

#endif