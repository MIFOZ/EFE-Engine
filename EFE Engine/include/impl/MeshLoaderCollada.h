#ifndef EFE_MESH_LOADER_COLLADA_H
#define EFE_MESH_LOADER_COLLADA_H

#include "math/MathTypes.h"
#include "resources/MeshLoader.h"
#include "graphics/VertexBuffer.h"

#include "system/MemoryManager.h"

class TiXmlElement;

namespace efe
{
	class cMesh;
	class cNode3D;
	class iVertexBuffer;
	class cBone;
	class cAnimation;
	class cAnimationTrack;
	class cSkeleton;
	class cWorld3D;
	class cSector;
	class cMeshEntity;
	class iPhysicsBody;
	class cColliderEntity;
	class cMeshJoint;
	class cSystem;

	class cColladaImage
	{
	public:
		tString m_sId;
		tString m_sName;

		tString m_sSource;
	};

	typedef std::vector<cColladaImage> tColladaImageVec;
	
	class cColladaTexture
	{
	public:
		tString m_sId;
		tString m_sName;

		tString m_sImage;
	};

	typedef std::vector<cColladaTexture> tColladaTextureVec;

	class cColladaMaterial
	{
	public:
		tString m_sId;
		tString m_sName;

		tString m_sTexture;
		cColor m_DiffuseColor;
	};

	typedef std::vector<cColladaMaterial> tColladaMaterialVec;

	class cColladaLight
	{
	public:
		tString m_sId;
		tString m_sName;
		tString m_sType;

		cColor m_DiffuseColor;
		float m_fAngle;
	};

	typedef std::vector<cColladaLight> tColladaLightVec;

	class cColladaVtxArray
	{
	public:
		cColladaVtxArray() : m_bIsInVertex(false){}

		tString m_sId;
		tString m_sType;
		bool m_bIsInVertex;

		tVector3fVec m_vArray;
	};

	typedef std::vector<cColladaVtxArray> tColladaVtxArrayVec;

	class cColladaVtxIndex
	{
	public:
		int m_lVtx;
		int m_lNorm;
		int m_lTex;
	};

	typedef std::vector<cColladaVtxIndex> tColladaVtxIndexVec;

	class cColladaExtraVtx
	{
	public:

		cColladaExtraVtx(){}

		int m_lVtx;
		int m_lNorm;
		int m_lTex;

		int m_lNewVtx;

		bool Equals(const cColladaVtxIndex &a_Data)
		{
			if (m_lVtx == a_Data.m_lVtx && m_lNorm== a_Data.m_lNorm && m_lTex == a_Data.m_lTex)
				return true;
			return false;
		}
	};

	typedef std::list<cColladaExtraVtx> tColladaExtraVtxList;
	typedef tColladaExtraVtxList::iterator tColladaExtraVtxListIt;

	typedef std::vector<tColladaExtraVtxList> tColladaExtraVtxListVec;

	class cColladaGeometry
	{
	public:
		cColladaGeometry() : m_lPosArrayIdx(-1), m_lNormArrayIdx(-1), m_lTexArrayIdx(-1),
						m_lPosIdxNum(-1), m_lNormIdxNum(-1), m_lTexIdxNum(-1){}

		void Clear()
		{
			m_vIndices.clear();

			for (int i=0;i<(int)m_vArrayVec.size();i++)
				m_vArrayVec[i].m_vArray.clear();
		}

		tString m_sId;
		tString m_sName;

		tVertexVec m_vVertexVec;
		tUIntVec m_vIndexVec;
		tColladaExtraVtxListVec m_vExtraVtxVec;
		tFloatVec m_vTangents;

		tString m_sMaterial;

		tColladaVtxArrayVec m_vArrayVec;
		tColladaVtxIndexVec m_vIndices;

		int m_lPosIdxNum;
		int m_lNormIdxNum;
		int m_lTexIdxNum;

		int m_lPosArrayIdx;
		int m_lNormArrayIdx;
		int m_lTexArrayIdx;
	};

	typedef std::vector<cColladaGeometry> tColladaGeometryVec;

	class cColladaJointPair
	{
	public:
		cColladaJointPair(){}
		cColladaJointPair(int a_lJoint, int a_lWeight)
		{
			m_lJoint = a_lJoint;
			m_lWeight = a_lWeight;
		}

		int m_lJoint;
		int m_lWeight;
	};

	typedef std::list<cColladaJointPair> tColladaJointPairList;
	typedef tColladaJointPairList::iterator tColladaJointPairListIt;

	typedef std::vector<tColladaJointPairList> tColladaJointPairListVec;

	class cColladaController
	{
	public:
		cColladaController() : m_lJointPairIdx(-1), m_lWeightPairIdx(-1){}

		tString m_sTarget;
		tString m_sId;

		int m_lJointPairIdx;
		int m_lWeightPairIdx;

		tStringVec m_vJoints;
		tFloatVec m_vWeights;

		tColladaJointPairListVec m_vPairs;
	};

	typedef std::vector<cColladaController> tColladaControllerVec;

	class cColladaChannel
	{
	public:
		tString m_sId;
		tString m_sTarget;
		tString m_sSource;
	};

	typedef std::vector<cColladaChannel> tColladaChannelVec;

	class cColladaSampler
	{
	public:
		tString m_sId;
		tString m_sTimeArray;
		tString m_sValueArray;

		tString m_sTarget;
	};

	typedef std::vector<cColladaSampler> tColladaSamplerVec;

	class cColladaAnimSource
	{
	public:
		tString m_sId;
		tFloatVec m_vValues;
	};

	typedef std::vector<cColladaAnimSource> tColladaAnimSourceVec;

	class cColladaAnimation
	{
	public:
		tString m_sId;
		tString m_sTargetNode;

		tColladaChannelVec m_vChannels;
		tColladaSamplerVec m_vSamplers;

		tColladaAnimSourceVec m_vSources;

		tFloatVec *GetSourceVec(const tString &a_sId)
		{
			for (size_t i=0; i<m_vSources.size(); i++)
			{
				if (m_vSources[i].m_sId == a_sId)
					return &m_vSources[i].m_vValues;
			}
			return NULL;
		}
	};
	typedef std::vector<cColladaAnimation> tColladaAnimationVec;

	class cColladaTransform
	{
	public:
		tString m_sId;
		tString m_sType;
		tFloatVec m_vValues;
	};

	typedef std::list<cColladaTransform> tColladaTransformList;
	typedef tColladaTransformList::iterator tColladaTransformListIt;

	class cColladaNode;
	typedef std::list<cColladaNode*> tColladaNodeList;
	typedef tColladaNodeList::iterator tColladaNodeListIt;

	class cColladaNode
	{
	public:
		cColladaNode() : m_lCount(0), m_pParent(0), m_vScale(1.0f, 1.0f, 1.0f){}

		tString m_sId;
		tString m_sName;
		tString m_sType;

		tString m_sSource;
		bool m_bSourceIsFile;

		cMatrixf m_mtxTransform;
		cMatrixf m_mtxWorldTransform;

		cColladaNode *m_pParent;

		cVector3f m_vScale;

		int m_lCount;

		tColladaNodeList m_lstChildren;

		tColladaTransformList m_lstTransforms;

		cColladaNode *CreateChild()
		{
			cColladaNode *pNode = efeNew(cColladaNode, ());
			m_lstChildren.push_back(pNode);
			pNode->m_pParent = this;
			return pNode;
		}

		cColladaTransform *GetTransform(const tString &a_sId)
		{
			tColladaTransformListIt it = m_lstTransforms.begin();
			for (; it != m_lstTransforms.end(); it++)
			{
				cColladaTransform &Trans = *it;
				if (Trans.m_sId == a_sId)
					return &Trans;
			}
			return NULL;
		}
	};

	class cColladaScene
	{
	public:
		cColladaScene()
		{
			m_fDeltaTime = 0;
		}

		~cColladaScene()
		{
			STLDeleteAll(m_lstNodes);
		}

		void ResetNodes()
		{
			STLDeleteAll(m_lstNodes);
			m_Root.m_lstChildren.clear();
		}

		cColladaNode *GetNode(const tString a_sId)
		{
			tColladaNodeListIt it = m_lstNodes.begin();
			for(;it != m_lstNodes.end(); ++it)
			{
				cColladaNode *pNode = *it;
				if (pNode->m_sId == a_sId || pNode->m_sName == a_sId)
					return pNode;
			}
			return NULL;
		}

		cColladaNode *GetNodeFromSource(const tString a_sSource)
		{
			tColladaNodeListIt it = m_lstNodes.begin();
			for(;it != m_lstNodes.end(); ++it)
			{
				cColladaNode *pNode = *it;
				if (pNode->m_sSource == a_sSource)
					return pNode;
			}
			return NULL;
		}

		float m_fStartTime;
		float m_fEndTime;
		float m_fDeltaTime;

		cColladaNode m_Root;

		tColladaNodeList m_lstNodes;
	};

	class cMeshLoaderCollada : public iMeshLoader
	{
	public:
		cMeshLoaderCollada(iLowLevelGraphics *a_pLowLevelGraphics);
		~cMeshLoaderCollada();

		cMesh *LoadMesh(const tString &a_sFile, tMeshLoadFlag a_Flags);
		bool SaveMesh(cMesh *a_pMesh, const tString &a_sFile){return false;};

		cWorld3D *LoadWorld(const tString &a_sFile, cScene *a_pScene, tWorldLoadFlag a_Flags);

		bool IsSupported(const tString a_sFileType);

		void AddSupportedTypes(tStringVec *a_vFileTypes);
	private:
		bool m_bZToY;

		tWorldLoadFlag m_Flags;

		void CreateHierarchyNodes(cMesh *a_pMesh, cNode3D *a_pParentNode,
			cColladaNode *a_pColladaNode,
			tColladaGeometryVec &a_vColladaGeom);

		cColladaGeometry *GetGeometry(const tString &a_sId, tColladaGeometryVec &a_vGeomVec);
		cColladaLight *GetLight(const tString &a_sId, tColladaLightVec &a_vLightVec);

		cMeshEntity *CreateStaticMeshEntity(cColladaNode *a_pNode, cWorld3D *a_pWorld,
			cColladaGeometry *a_pGeom, bool a_bInRoomGroup,
			tColladaMaterialVec &a_vColladaMaterials,
			tColladaTextureVec &a_vColladaTextures,
			tColladaImageVec &a_vColladaImages);

		cColliderEntity *CreateStaticCollider(cColladaNode *a_pNode, cWorld3D *a_pWorld,
			cColladaGeometry *a_pGeom,
			tColladaMaterialVec &a_vColladaMaterials,
			tColladaTextureVec &a_vColladaTextures,
			tColladaImageVec &a_vColladaImages,
			bool a_bCharacterCollider);

		void AddSceneObjects(cColladaNode *a_pNode, cWorld3D *a_pWorld,
			tColladaGeometryVec &a_vColladaGeomtries, tColladaLightVec &a_vColladaLights,
			tColladaMaterialVec &a_vColladaMaterials, tColladaTextureVec &a_vColladaTextures,
			tColladaImageVec &a_vColladaImages, cColladaScene *a_pColladaScene);

		cAnimationTrack *CreateAnimTrack(cAnimation *a_pAnimation, cSkeleton *a_pSkeleton,
			cColladaAnimation &a_Anim, cColladaScene *a_pScene);

		iVertexBuffer *CreateVertexBuffer(cColladaGeometry &a_Geometry,
											eVertexBufferUsageType a_UsageType);

		bool FillStructures(const tString &a_sFile,
							tColladaImageVec *a_pColladaImageVec,
							tColladaTextureVec *a_pColladaTextureVec,
							tColladaMaterialVec *a_pColladaMaterialVec,
							tColladaLightVec *a_pColladaLightVec,
							tColladaGeometryVec *a_pColladaGeometryVec,
							tColladaControllerVec *a_pColladaontrollerVec,
							tColladaAnimationVec *a_pColladaAnimVec,
							cColladaScene *a_pColladaScene,
							bool a_bCache);
		void LoadColladaScene(TiXmlElement *a_pRootElem, cColladaNode *a_pParentNode, cColladaScene *a_pScene,
								tColladaLightVec *a_pColladaLightVec);

		void LoadAnimations(TiXmlElement *a_pRootElem, tColladaAnimationVec &a_vAnimations,
			cColladaScene *a_pColladaScene);

		void LoadImages(TiXmlElement *a_pRootElem, tColladaImageVec &a_vColladaImageVec);

		void LoadTextures(TiXmlElement *a_pRootElem, tColladaTextureVec &a_vColladaTextureVec);

		void LoadMaterials(TiXmlElement *a_pRootElem, tColladaMaterialVec &a_vColladaMaterialVec);

		void LoadLights(TiXmlElement *a_pRootElem, tColladaLightVec &a_vColladaLightVec);

		void LoadGeometry(TiXmlElement *a_pRootElem, tColladaGeometryVec &a_vColladaGeometryVec, cColladaScene *a_pColladaScene);
		void LoadVertexData(TiXmlElement *a_pSourceElem, tVector3fVec &a_vVtxVec);

		//Helpers
		void SplitVertices(cColladaGeometry &a_Geometry, tColladaExtraVtxListVec &a_vExtraVtxVec,
							tVertexVec &a_vVertexVec, tUIntVec &a_vIndexVec);

		void FillVertexVec(const char *a_pChars, tVector3fVec &a_vVtxVec, int a_lElements, int a_lVtxCount);

		tString GetMaterialTextureFile(const tString &a_sMaterial, tColladaMaterialVec &a_vColladaMaterialVec,
																tColladaTextureVec &a_vColladaTextureVec,
																tColladaImageVec &a_vColladaImageVec);

		cVector3f GetVectorPos(const cVector3f &a_vVec);
		cVector3f GetVectorPosFromPtr(float *a_pVec);

		//Vertex Format
		iVertexFormat *m_pVertexFormat;
	};
};
#endif