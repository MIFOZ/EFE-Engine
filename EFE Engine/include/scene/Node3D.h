#ifndef EFE_NODE3D_H
#define EFE_NODE3D_H

#include <list>
#include "math/MathTypes.h"
#include "scene/Node.h"

namespace efe
{
	kSaveData_ChildClass(iNode, cNode3D)
	{
		kSaveData_ClassInit(cNode3D)
	public:
		tString m_sName;
		tString m_sSource;
		bool m_bAutoDeleteChildren;
		cMatrixf m_mtxLocalTransform;
		int m_lParentId;

		iSaveObject *CreateSaveObject(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);
		int GetSaveCreatePrio();
	};

	class cNode3D : public iNode
	{
	public:
		cNode3D(const tString &a_sName="", bool a_bAutoDeleteChildren = true);
		virtual ~cNode3D();

		iNode *CreateChild();
		cNode3D *CreateChild3D(const tString &a_sName="", bool a_bAutoDeleteChildren = true);

		cVector3f GetLocalPosition();
		cMatrixf &GetLocalMatrix();

		cVector3f GetWorldPosition();
		cMatrixf &GetWorldMatrix();

		void SetPosition(const cVector3f &a_vPos);
		void SetMatrix(const cMatrixf &a_mtxTransform, bool a_bSetChildrenUpdated = true);

		void SetWorldPosition(const cVector3f &a_vWorldPos);
		void SetWorldMatrix(const cMatrixf &a_mtxWorldTransform);

		const char *GetName();

		cNode3D *GetParent();

		void AddRotation(const cQuaternion &a_qRotation);

		void AddScale(const cVector3f &a_vScale);

		void AddTranslation(const cVector3f &a_vTrans);

		void SetSource(const tString &a_sSource);
		const char *GetSource();

		//************************************
		// Method:    UpdateMatrix
		// FullName:  efe::cNode3D::UpdateMatrix
		// Access:    public 
		// Returns:   void
		// Qualifier: Updates the matrix with added scales, translations, rotations. It also resets these values.
		// Parameter: bool a_bSetChildrenUpdated
		//************************************
		void UpdateMatrix(bool a_bSetChildrenUpdated);

		void SetParent(cNode3D *a_pNode);
		void AddChild(cNode3D *a_pNode);

		virtual iSaveData *CreateSaveData();
		virtual void SaveToSaveData(iSaveData *a_pSaveData);
		virtual void LoadFromSaveData(iSaveData *a_pSaveData);
		virtual void SaveDataSetup(cSaveObjectHandler *a_pSaveObjectHandler, cGame *a_pGame);

		void UpdateWorldTransform();
		void SetWorldTransformUpdated();
	private:
		tString m_sName;
		tString m_sSource;

		bool m_bAutoDeleteChildren;

		cMatrixf m_mtxLocalTransform;
		cMatrixf m_mtxWorldTransform;

		cVector3f m_vWorldPosition;

		cMatrixf m_mtxRotation;
		cVector3f m_vScale;
		cVector3f m_vTranslation;
		cQuaternion m_qOrientation;

		bool m_bTransformUpdated;

		cNode3D *m_pParent;
	};
};
#endif