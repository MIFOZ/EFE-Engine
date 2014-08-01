#ifndef EFE_RENDER_LIST_H
#define EFE_RENDER_LIST_H

#include <set>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "scene/Light3D.h"
#include "graphics/Material.h"

#include "graphics/VertexBuffer.h"


namespace efe
{
	class iRenderable;
	class cCamera3D;
	class cGraphics;

	class iRenderState;
	class cRenderSettings;

	enum eRenderListDrawType
	{
		eRenderListDrawType_Normal,
		eRenderListDrawType_Trans,
		eRenderListDrawType_LastEnum
	};

	class cRenderNode;

	class cRenderNodeCompare
	{
	public:
		bool operator()(cRenderNode *a_pNodeA, cRenderNode *a_pNodeB) const;
	};

	typedef std::set<cRenderNode *, cRenderNodeCompare> tRenderNodeSet;
	typedef tRenderNodeSet::iterator tRenderNodeSetIt;

	class cRenderNode
	{
	public:
		cRenderNode() : m_pState(NULL){}

		iRenderState *m_pState;

		tRenderNodeSet m_setNodes;

		void DeleteChildren();
		void Render(cRenderSettings *a_pSettings);
	};

	class cTransparentObject_Compare
	{
	public:
		bool operator()(iRenderable *a_pObjectA, iRenderable *a_pObjectB) const;
	};

	typedef std::multiset<iRenderable*,cTransparentObject_Compare> tTransparentObjectSet;
	typedef tTransparentObjectSet::iterator tTransparentObjectSetIt;

	typedef std::set<iRenderable*> tRenderableSet;
	typedef tRenderableSet::iterator tRenderableSetIt;

	typedef std::set<iLight3D*> tLight3DSet;
	typedef tLight3DSet::iterator tLight3DSetIt;

	typedef cSTLIterator<iLight3D*, tLight3DSet, tLight3DSetIt> cLight3DIterator;
	typedef cSTLIterator<iRenderable*, tRenderableSet, tRenderableSetIt> cRenderableIterator;

	class cRenderList
	{
	public:
		cRenderList(cGraphics *a_pGraphics);
		~cRenderList();

		void SetCamera(cCamera3D *a_pCamera){m_pCamera = a_pCamera;}
		cCamera3D *GetCamera(){return m_pCamera;}

		bool Add(iRenderable *a_pObject);
		void Clear();

		cLight3DIterator GetLightIt();
		cRenderableIterator GetObjectIt();

		int GetLightNum();
		int GetObjectNum();

		void Compile();

		int GetRenderCount(){return m_lRenderCount;}
		int GetLastRenderCount(){return m_lLastRenderCount;}

		void SetFrameTime(float a_fTime){m_fFrameTime = a_fTime;}

		cRenderNode *GetRootNode(eRenderListDrawType a_ObjectType, eMaterialRenderType a_PassType, int a_lLightNum);

		static __forceinline int GetGlobalRenderCount(){return m_lGlobalRenderCount;}
	private:
		cRenderNode *InsertNode(cRenderNode *a_pListNode, cRenderNode *a_pTempNode);

		void AddToTree(iRenderable *a_pObject, eRenderListDrawType a_ObjectType,
						eMaterialRenderType a_PassType, int a_lLightNum, iLight3D *a_pLight,
						bool a_bUseDepth, int a_lPass);

		static int m_lGlobalRenderCount;

		tLight3DSet m_setLights;
		int m_vObjectsPerLight[MAX_NUM_OF_LIGHTS];

		tRenderableSet m_setObjects;

		tTransparentObjectSet m_setTransparentObjects;

		cRenderNode m_RootNodeDepth;
		cRenderNode m_RootNodeDiffuse;
		cRenderNode m_RootNodeLight[MAX_NUM_OF_LIGHTS];
		cRenderNode m_RootNodeTrans;

		cRenderNode m_TempNode;

		int m_lRenderCount;
		int m_lLastRenderCount;

		float m_fFrameTime;

		cMemoryPool<iRenderState> *m_poolRenderState;
		cMemoryPool<cRenderNode> *m_poolRenderNode;

		cCamera3D *m_pCamera;

		cGraphics *m_pGraphics;
	};
};
#endif