#ifndef EFE_ENTITY3D_H
#define EFE_ENTITY3D_H

#include <list>
#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "scene/Entity.h"
#include "system/Container.h"

#include "math/BoundingVolume.h"

namespace efe
{
	class iEntity3D;
	class cSector;

	class iEntityCallback
	{
	public:
		virtual void OnTransformUpdate(iEntity3D *a_pEntity)=0;
	};

	typedef std::list<iEntityCallback*> tEntityCallbackList;
	typedef tEntityCallbackList::iterator tEntityCallbackListIt;

	class iRenderContainerData
	{
		
	};

	typedef std::list<iRenderContainerData*> tRenderContainerDataList;
	typedef tRenderContainerDataList::iterator tRenderContainerDataListIt;

	kSaveData_ChildClass(iEntity, iEntity3D)
	{
		kSaveData_ClassInit(iEntity3D)
	public:
		cMatrixf m_mtxLocalTransform;
		cBoundingVolume m_BoundingVolume;
		tString m_sSourceFile;

		int m_lParentId;
		cContainerList<int> m_lstChildren;
	};

	class iEntity3D;

	typedef std::list<iEntity3D*> tEntity3DList;
	typedef tEntity3DList::iterator tEntity3DListIt;

	class iEntity3D : public iEntity
	{
	public:
		iEntity3D(tString a_sName);
		virtual ~iEntity3D();

		cVector3f GetLocalPosition();
		cMatrixf &GetLocalMatrix();

		cVector3f GetWorldPosition();
		cMatrixf &GetWorldMatrix();

		void SetPosition(const cVector3f &a_vPos);
		void SetMatrix(const cMatrixf &a_mtxTransform);

		void SetWorldPosition(const cVector3f &a_vWorldPos);
		void SetWorldMatrix(const cMatrixf &a_mtxWorldTransform);

		void SetTransformUpdated(bool a_bUpdatedCallbacks = true);
		bool GetTransformUpdated();

		int GetTransformUpdateCount();

		void AddCallback(iEntityCallback *a_pCallback);
		void RemoveCallback(iEntityCallback *a_pCallback);

		void SetSourceFile(const tString &a_sFile){m_sSourceFile = a_sFile;}
		const tString &GetSourceFile(){return m_sSourceFile;}

		virtual cBoundingVolume *GetBoundingVolume(){return &m_BoundingVolume;}

		void AddChild(iEntity3D *a_pEntity);

		virtual tRenderContainerDataList *GetRenderContainerDataList(){return &m_lstRenderContainerData;}

		__forceinline int GetIteratorCount(){return m_lIteratorCount;}
		__forceinline void SetIteratorCount(const int a_lX){m_lIteratorCount = a_lX;}

		__forceinline void SetCurrentSector(cSector *a_pSector){m_pCurrentSector = a_pSector;}
		__forceinline cSector *GetCurrentSector() const{return m_pCurrentSector;}

		bool IsInSector(cSector *a_pSector);

		__forceinline int GetGlobalRenderCount(){return m_lGlobalRenderCount;}
		__forceinline void SetGlobalRenderCount(int a_lX){m_lGlobalRenderCount = a_lX;}

		//SaveObject implementation
		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

	protected:
		cMatrixf m_mtxLocalTransform;
		cMatrixf m_mtxWorldTransform;

		cBoundingVolume m_BoundingVolume;
		bool m_bUpdateBoundingVolume;
		bool m_bApplyTransformToBV;

		bool m_bTransformUpdated;

		int m_lGlobalRenderCount;

		int m_lCount;

		tString m_sSourceFile;

		tEntityCallbackList m_lstCallbacks;

		tRenderContainerDataList m_lstRenderContainerData;

		tEntity3DList m_lstChildren;
		iEntity3D *m_pParent;

		cSector *m_pCurrentSector;

		int m_lIteratorCount;
	private:
		void UpdateWorldTransform();
	};
};

#endif