#include "impl/MeshLoaderCollada.h"

#include "system/LowLevelSystem.h"
#include "Graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "system/String.h"

#include "graphics/Mesh.h"

#include "resources/MaterialManager.h"
#include "graphics/Material.h"

#include "graphics/Bone.h"
#include "graphics/Skeleton.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"

#include "impl/tinyXML/tinyxml.h"

#include "math/Math.h"

namespace efe
{
#define GetAddress(sStr) if (sStr.length()>0 && sStr[0]=='#') sStr = cString::Sub(sStr, 1)
	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	cVector3f cMeshLoaderCollada::GetVectorPos(const cVector3f &a_vVec)
	{
		if (m_bZToY)
			return cVector3f(-a_vVec.x, a_vVec.z, a_vVec.y);
		else
			return a_vVec;
	}

	cVector3f cMeshLoaderCollada::GetVectorPosFromPtr(float *a_pVec)
	{
		if (m_bZToY)
			return cVector3f(-a_pVec[0], a_pVec[2], a_pVec[1]);
		else
			return cVector3f(a_pVec[0], a_pVec[1], a_pVec[2]);
	}

	//--------------------------------------------------------------

	iVertexBuffer *cMeshLoaderCollada::CreateVertexBuffer(cColladaGeometry &a_Geometry,
		eVertexBufferUsageType a_UsageType)
	{
		tVertexFlag Flags = eVertexFlag_Position | eVertexFlag_Normal | eVertexFlag_Color0;
		bool bHasTexCoords = false;
		if (a_Geometry.m_lTexIdxNum != -1)
		{
			Flags |= (eVertexFlag_Texture0 | eVertexFlag_Tangent);
			bHasTexCoords = true;
		}
		iVertexBuffer *pVtxBuffer = m_pLowLevelGraphics->CreateVertexBuffer(Flags,
			eVertexBufferDrawType_TriangleList, a_UsageType,
			(int)a_Geometry.m_vVertexVec.size(), (int)a_Geometry.m_vIndexVec.size());

		if (bHasTexCoords)
		{
			pVtxBuffer->SetTangents(true);
			pVtxBuffer->ResizeArray(eVertexFlag_Tangent, (int)a_Geometry.m_vTangents.size());
		}

		for (size_t j = 0; j < a_Geometry.m_vVertexVec.size(); j++)
		{
			pVtxBuffer->AddVertex(eVertexFlag_Position, a_Geometry.m_vVertexVec[j].pos);
			pVtxBuffer->AddVertex(eVertexFlag_Normal, a_Geometry.m_vVertexVec[j].norm);

			if (bHasTexCoords)
				pVtxBuffer->AddVertex(eVertexFlag_Texture0, a_Geometry.m_vVertexVec[j].tex);

			pVtxBuffer->AddColor(eVertexFlag_Color0, cColor(1,1));
		}

		if (bHasTexCoords)
		{
			memcpy(pVtxBuffer->GetArray(eVertexFlag_Tangent), &a_Geometry.m_vTangents[0],
				a_Geometry.m_vTangents.size()*sizeof(float));
		}

		for (size_t j = 0; j < a_Geometry.m_vIndexVec.size(); j++)
		{
			size_t lIdx = (j/3)*3 + (2-(j%3));

			pVtxBuffer->AddIndex(a_Geometry.m_vIndexVec[lIdx]);
		}

		if (pVtxBuffer->Compile(0) == false)
			FatalError("Could not create vertex buffer for '%s'\n", a_Geometry.m_sName.c_str());

		return pVtxBuffer;
	}

	//--------------------------------------------------------------

	class cTempAnimData
	{
	public:
		cTempAnimData() : m_vTrans(0, 0, 0), m_vRot(0, 0, 0), m_vScale(1, 1, 1) {}

		cVector3f m_vTrans;
		cVector3f m_vRot;
		cVector3f m_vScale;
		eKeyFrameType m_InterpolationType;

		cVector2f m_vOutAnchor;
		cVector2f m_vInAnchor;

		float m_fTime;

		int m_lIndex;
	};

	typedef std::vector<cTempAnimData> tTempAnimDataVec;

	typedef std::set<float> tTempTimesSet;
	typedef tTempTimesSet::iterator tTempTimesSetIt;

	static cTempAnimData *GetTempAnimData(float a_fTime, tTempAnimDataVec &a_vTempData)
	{
		for (size_t i=0; i<a_vTempData.size(); ++i)
		{
			if (a_vTempData[i].m_fTime == a_fTime) return &a_vTempData[i];
		}

		return NULL;
	}

	static void GetAnimTimes(float a_fTime, float *a_pTimeBefore, float *a_pTimeAfter, tFloatVec *a_pTimeVec)
	{
		*a_pTimeBefore = -1; *a_pTimeAfter = -1;
		for (size_t i=0; i<a_pTimeVec->size(); ++i)
		{
			float fTime = (*a_pTimeVec)[i];

			if (fTime <= a_fTime)
				*a_pTimeBefore = fTime;
			else if (fTime >= a_fTime)
				*a_pTimeAfter = fTime;
		}

	}

	static eKeyFrameType GetKeyFrameTypeEnumFromString(const tString &a_sType)
	{
		tString sLow = cString::ToLowerCase(a_sType);
		if (sLow == "linear")		return eKeyFrameType_Linear;
		else if (sLow == "bezier")	return eKeyFrameType_Bezier;
		else						return eKeyFrameType_Linear;
	}

	static void GetBezierAnchorPoints(int a_lIndex, tFloatVec *a_pInTangents, tFloatVec *a_pOutTangents, cVector2f *a_pOut, cVector2f *a_pIn)
	{
		*a_pOut = (*a_pOutTangents)[a_lIndex];
		*a_pIn = (*a_pInTangents)[a_lIndex+1];
	}

	cAnimationTrack *cMeshLoaderCollada::CreateAnimTrack(cAnimation *a_pAnimation, cSkeleton *a_pSkeleton,
		cColladaAnimation &a_Anim, cColladaScene *a_pScene)
	{
		tTempAnimDataVec vTempData;
		tTempTimesSet setTempTimes;

		cColladaNode *pNode = a_pScene->GetNode(a_Anim.m_sTargetNode);
		if (pNode == NULL)
		{
			Error("Couldn't find node '%s' for animation id '%s'\n", a_Anim.m_sTargetNode.c_str(), a_Anim.m_sId.c_str());
			return NULL;
		}

		cBone *pBone = NULL;

		if (a_pSkeleton)
		{
			pBone = a_pSkeleton->GetBoneByName(a_Anim.m_sTargetNode);
			if (pBone == NULL)
			{
				Error("Couldn't find bone '%s'\n", a_Anim.m_sTargetNode.c_str());
				return NULL;
			}
		}

		for (size_t i=0; i<a_Anim.m_vSamplers.size(); i++)
		{
			cColladaSampler &Sampler = a_Anim.m_vSamplers[i];

			tFloatVec *pValueVec = a_Anim.GetSourceVec(Sampler.m_sTimeArray);
			if (pValueVec == NULL)
			{
				Error("Time array not found in sampler  '%s'\n", a_Anim.m_sTargetNode.c_str());
				return NULL;
			}

			for (tFloatVecIt it = pValueVec->begin(); it != pValueVec->end(); ++it)
			{
				setTempTimes.insert(*it);
			}
		}

		vTempData.resize(setTempTimes.size());
		int lCount = 0;
		for (tTempTimesSetIt it = setTempTimes.begin(); it != setTempTimes.end(); ++it, ++lCount)
		{
			vTempData[lCount].m_fTime = *it;
			vTempData[lCount].m_lIndex = lCount;
		}

		for (size_t i=0; i<a_Anim.m_vSamplers.size(); i++)
		{
			cColladaSampler &Sampler = a_Anim.m_vSamplers[i];

			tFloatVec *pTimeVec = a_Anim.GetSourceVec(Sampler.m_sTimeArray);
			if (pTimeVec == NULL)
			{
				Error("Time array not found!\n");
				return NULL;
			}

			tString sTarget = cString::SetFileExt(cString::GetFileName(Sampler.m_sTarget),"");
			tString sExt = cString::ToLowerCase(cString::GetFileExt(Sampler.m_sTarget));

			if (m_bZToY)
			{
				if (sExt == "z") sExt = "y";
				else if (sExt == "y") sExt = "z";
			}

			cColladaTransform *pTrans = pNode->GetTransform(sTarget);
			if (pTrans == NULL)
				continue;

			tFloatVec *pValueVec = a_Anim.GetSourceVec(Sampler.m_sValueArray);
			if (pValueVec == NULL)
			{
				Error("Value array not found!\n");
				return NULL;
			}
			tStringVec *pInterpolations = a_Anim.GetInterpolationVec(Sampler.m_sInterpolationArray);
			tFloatVec *pInTangents = a_Anim.GetSourceVec(Sampler.m_sInTangentArray);
			tFloatVec *pOutTangents = a_Anim.GetSourceVec(Sampler.m_sOutTangentArray);

			if (pTrans->m_sType == "translate")
			{
				for (size_t j=0; j<pTimeVec->size(); j++)
				{
					cVector3f vTrans = pNode->m_mtxTransform.GetTranslation();

					cTempAnimData *pTempData = GetTempAnimData((*pTimeVec)[j], vTempData);
					if (pTempData == NULL) {Error("Code error at %d\n", __LINE__); return NULL;}

					if (sExt == "")
					{
						pTempData->m_vTrans = cVector3f((*pValueVec)[j*3 + 0],
							(*pValueVec)[j*3 + 1],
							(*pValueVec)[j*3 + 2]);
					}
					else if (sExt == "x")
					{
						pTempData->m_vTrans = cVector3f((*pValueVec)[j],vTrans.y,vTrans.z);
					}
					else if (sExt == "y")
					{
						pTempData->m_vTrans = cVector3f(vTrans.x,(*pValueVec)[j],vTrans.z);
					}
					else if (sExt == "z")
					{
						pTempData->m_vTrans = cVector3f(vTrans.x,vTrans.y,(*pValueVec)[j]);
					}

					if (pBone)
						pTempData->m_vTrans -= pBone->GetLocalTransform().GetTranslation();

					// Interpolation
					pTempData->m_InterpolationType = GetKeyFrameTypeEnumFromString((*pInterpolations)[j]);
					// Add anchor data
					if (pTempData->m_InterpolationType == eKeyFrameType_Bezier)
					{
						pTempData->m_vOutAnchor = cVector2f((*pOutTangents)[j*2 + 0], (*pOutTangents)[j*2 + 1]);
						pTempData->m_vInAnchor = cVector2f((*pInTangents)[j*2 + 0], (*pInTangents)[j*2 + 1]);
					}
				}

				float fTimeBefore = -1, fTimeAfter = -1;
				for (size_t j=0; j<vTempData.size(); ++j)
				{
					GetAnimTimes(vTempData[j].m_fTime, &fTimeBefore, &fTimeAfter, pTimeVec);

					//Time exists
					if (fTimeBefore == vTempData[j].m_fTime)
						continue;
					else if (fTimeBefore >= 0 && fTimeAfter >= 0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

						float fT = (vTempData[j].m_fTime - pBefore->m_fTime) / (pAfter->m_fTime - pBefore->m_fTime);

						vTempData[j].m_vTrans = pBefore->m_vTrans*(1.0f-fT) + pAfter->m_vTrans*fT;
					}
					else if (fTimeBefore < 0 && fTimeAfter >= 0)
					{
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

						vTempData[j].m_vTrans = pAfter->m_vTrans;
					}
					else if (fTimeBefore >= 0 && fTimeAfter < 0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);

						vTempData[j].m_vTrans = pBefore->m_vTrans;
					}
				}
			}
			else if (pTrans->m_sType == "rotate")
			{
				for (size_t j=0; j<pTimeVec->size(); j++)
				{
					cTempAnimData *pTempData = GetTempAnimData((*pTimeVec)[j], vTempData);
					if (pTempData == NULL) {Error("Code error at %d\n", __LINE__); return NULL;}

					pTempData->m_vRot += cVector3f(pTrans->m_vValues[0],
						pTrans->m_vValues[1], pTrans->m_vValues[2]) * (*pTimeVec)[j];
				}

				size_t lVecNum = 0;
				if (pTrans->m_vValues[0]>0.001f) lVecNum = 0;
				else if (pTrans->m_vValues[1]>0.001f) lVecNum = 1;
				else if (pTrans->m_vValues[2]>0.001f) lVecNum = 2;

				float fTimeBefore = -1, fTimeAfter = -1;
				for (size_t j=0; j<vTempData.size(); ++j)
				{
					GetAnimTimes(vTempData[j].m_fTime, &fTimeBefore, &fTimeAfter, pTimeVec);

					if (fTimeBefore == vTempData[j].m_fTime)
						continue;
					else if (fTimeBefore >= 0 && fTimeAfter >= 0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

						float fT = (vTempData[j].m_fTime - pBefore->m_fTime) / (pAfter->m_fTime - pBefore->m_fTime);

						vTempData[j].m_vRot.v[lVecNum] = pBefore->m_vRot.v[lVecNum]*(1.0f-fT) + 
							pAfter->m_vRot.v[lVecNum]*fT;
					}
					else if (fTimeBefore < 0 && fTimeAfter >= 0)
					{
						cTempAnimData *pAfter = GetTempAnimData(fTimeAfter, vTempData);

						vTempData[j].m_vRot.v[lVecNum] = pAfter->m_vRot.v[lVecNum];
					}
					else if (fTimeBefore >= 0 && fTimeAfter < 0)
					{
						cTempAnimData *pBefore = GetTempAnimData(fTimeBefore, vTempData);

						vTempData[j].m_vRot.v[lVecNum] = pBefore->m_vRot.v[lVecNum];
					}
				}
			}
		}

		cAnimationTrack *pTrack = a_pAnimation->CreateTrack(a_Anim.m_sTargetNode,
			eAnimTransformFlag_Rotate |
			eAnimTransformFlag_Translate |
			eAnimTransformFlag_Scale);

		for (size_t i=0; i<vTempData.size(); i++)
		{
			cKeyFrame *pFrame = pTrack->CreateKeyFrame(vTempData[i].m_fTime - a_pScene->m_fStartTime);

			pFrame->trans = vTempData[i].m_vTrans;
			pFrame->scale = vTempData[i].m_vScale;

			cVector3f vRadRot = vTempData[i].m_vRot;
			vRadRot = cVector3f(cMath::ToRad(vRadRot.x), cMath::ToRad(vRadRot.y), cMath::ToRad(vRadRot.z));
			cMatrixf mtxRot = cMath::MatrixRotate(vRadRot, eEulerRotationOrder_XYZ);
			cQuaternion qRot;
			qRot.FromRotationMatrix(mtxRot);

			pFrame->rotation = qRot;

			pFrame->type = vTempData[i].m_InterpolationType;
			pFrame->outAnchor = vTempData[i].m_vOutAnchor;
			pFrame->inAnchor = vTempData[i].m_vInAnchor;
		}

		return pTrack;
	}

	//--------------------------------------------------------------

	void cMeshLoaderCollada::LoadColladaScene(TiXmlElement *a_pRootElem, cColladaNode *a_pParentNode, cColladaScene *a_pScene,
		tColladaLightVec *a_pColladaLightVec)
	{
		cColladaNode *pNode = a_pParentNode->CreateChild();
		a_pScene->m_lstNodes.push_back(pNode);

		//The local matrix
		cMatrixf mtxTransform;
		mtxTransform = cMatrixf::Identity;

		//Vector to save transform values in later on
		tFloatVec vValVec;
		vValVec.reserve(5);

		pNode->m_sId = cString::ToString(a_pRootElem->Attribute("id"),"");
		pNode->m_sName = cString::ToString(a_pRootElem->Attribute("name"),"");
		pNode->m_sType = cString::ToString(a_pRootElem->Attribute("type"),"");

		TiXmlElement *pInstanceElem = a_pRootElem->FirstChildElement("instance_geometry");
		if (pInstanceElem==NULL) pInstanceElem = a_pRootElem->FirstChildElement("instance_light");
		if (pInstanceElem==NULL) pInstanceElem = a_pRootElem->FirstChildElement("instance_controller");
		if (pInstanceElem==NULL) pInstanceElem = a_pRootElem->FirstChildElement("instance");
		if (pInstanceElem)
		{
			tString sSource = cString::ToString(pInstanceElem->Attribute("url"),"");
			if (sSource[0] == '#') pNode->m_bSourceIsFile = false;
			else					pNode->m_bSourceIsFile = true;

			GetAddress(sSource);

			pNode->m_sSource = sSource;
		}

		Log("Node. %s, type: %s\n", pNode->m_sId.c_str(), pNode->m_sType.c_str());

		cVector3f vTranslation = cVector3f(0,0,0);

		TiXmlElement *pTransformElem = a_pRootElem->FirstChildElement();
		while(pTransformElem)
		{
			tString sVal = pTransformElem->Value();
			tString sSid = cString::ToString(pTransformElem->Attribute("sid"),"");

			TiXmlNode *pChildElem = pTransformElem->FirstChild();
			if (pChildElem == NULL)
			{
				pTransformElem = pTransformElem->NextSiblingElement();
				continue;
			}

			Log("val: %s\n", sVal.c_str());

			TiXmlText *pText = pChildElem->ToText();
			if (pText==NULL)
			{
				pTransformElem = pTransformElem->NextSiblingElement();
				continue;
			}

			cString::GetFloatVec(pText->Value(), vValVec);

			//Translation
			if (sVal=="translate")
			{
				cVector3f vTrans = GetVectorPosFromPtr(&vValVec[0]);

				//D3DXMatrixMultiply(&mtxTransform, &mtxTransform, 
			}
			//Rotation
			if (sVal=="rotation")
			{

				cQuaternion qRot;
				cVector3f vRotAxis = GetVectorPosFromPtr(&vValVec[0]);

				qRot.FromAngleAxis(cMath::ToRad(vValVec[3]),vRotAxis);

				mtxTransform = cMath::MatrixMul(mtxTransform, cMath::MatrixQuaternion(qRot));
			}
			//Scaling
			else if(sVal=="scale")
			{

			}

			pNode->m_lstTransforms.push_back(cColladaTransform());
			cColladaTransform &Transform = pNode->m_lstTransforms.back();

			Transform.m_sId = sSid;
			Transform.m_vValues = vValVec;
			Transform.m_sType = sVal;

			vValVec.clear();
			pTransformElem = pTransformElem->NextSiblingElement();
		}

		pNode->m_mtxTransform = mtxTransform;
		pNode->m_mtxWorldTransform = cMath::MatrixMul(a_pParentNode->m_mtxWorldTransform, mtxTransform);

		TiXmlElement *pNodeElem = a_pRootElem->FirstChildElement("node");
		while(pNodeElem)
		{
			LoadColladaScene(pNodeElem,pNode,a_pScene,a_pColladaLightVec);

			pNodeElem = pNodeElem->NextSiblingElement("node");
		}
	}

	static tVector3fVec *g_pVertexVec = NULL;

	class cColladaTestTri
	{
	public:
		cColladaTestTri(cColladaVtxIndex *a_pTriIndex)
		{
			//Log("TestTri:\n");
			//Get the vectors
			for (int i=0;i<3;i++)
			{
				m_vPos[i] = (*g_pVertexVec)[a_pTriIndex[i].m_lVtx];
				//Log("Pos%d: %s\n", i, cString::ToString(m_vPos[i]
			}

			//Sort the vectors
			for (int i=0;i<3;i++)
			{
				for (int j=0;j<3;j++)
				{
					if (j==i)continue;

					if (m_vPos[i] < m_vPos[j])
					{
						cVector3f vTemp = m_vPos[i];
						m_vPos[i] = m_vPos[j];
						m_vPos[j] = vTemp;
					}
				}
			}	
		}

		cVector3f m_vPos[3];
	};

	class cColladaTestTriCompare
	{
	public:
		bool operator()(const cColladaTestTri &a_Tri1, const cColladaTestTri &a_Tri2)
		{
			if (a_Tri1.m_vPos[0].x != a_Tri2.m_vPos[0].x) return a_Tri1.m_vPos[0].x < a_Tri2.m_vPos[0].x;
			if (a_Tri1.m_vPos[0].y != a_Tri2.m_vPos[0].y) return a_Tri1.m_vPos[0].y < a_Tri2.m_vPos[0].y;
			if (a_Tri1.m_vPos[0].z != a_Tri2.m_vPos[0].z) return a_Tri1.m_vPos[0].z < a_Tri2.m_vPos[0].z;

			if (a_Tri1.m_vPos[1].x != a_Tri2.m_vPos[1].x) return a_Tri1.m_vPos[1].x < a_Tri2.m_vPos[1].x;
			if (a_Tri1.m_vPos[1].y != a_Tri2.m_vPos[1].y) return a_Tri1.m_vPos[1].y < a_Tri2.m_vPos[1].y;
			if (a_Tri1.m_vPos[1].z != a_Tri2.m_vPos[1].z) return a_Tri1.m_vPos[1].z < a_Tri2.m_vPos[1].z;

			if (a_Tri1.m_vPos[2].x != a_Tri2.m_vPos[2].x) return a_Tri1.m_vPos[2].x < a_Tri2.m_vPos[2].x;
			if (a_Tri1.m_vPos[2].y != a_Tri2.m_vPos[2].y) return a_Tri1.m_vPos[2].y < a_Tri2.m_vPos[2].y;
			if (a_Tri1.m_vPos[2].z != a_Tri2.m_vPos[2].z) return a_Tri1.m_vPos[2].z < a_Tri2.m_vPos[2].z;

			return false;
		}
	};

	typedef std::set<cColladaTestTri, cColladaTestTriCompare> tColladaTestTriMap;
	typedef tColladaTestTriMap::iterator tColladaTestTriMapIt;

	void cMeshLoaderCollada::LoadGeometry(TiXmlElement *a_pRootElem, tColladaGeometryVec &a_vColladaGeometryVec, cColladaScene *a_pColladaScene)
	{
		TiXmlElement *pGeomElem = a_pRootElem->FirstChildElement("geometry");
		for (;pGeomElem != NULL;pGeomElem = pGeomElem->NextSiblingElement("geometry"))
		{
			a_vColladaGeometryVec.push_back(cColladaGeometry());

			cColladaGeometry &Geometry = a_vColladaGeometryVec[a_vColladaGeometryVec.size()-1];

			Geometry.m_sId = cString::ToString(pGeomElem->Attribute("id"),"");
			Geometry.m_sName = cString::ToString(pGeomElem->Attribute("name"),"");

			if (Geometry.m_sName=="")
			{
				Geometry.m_sName = Geometry.m_sId;
				int lPos = cString::GetLastStringPos(Geometry.m_sName, "-");
				if (lPos > 0) Geometry.m_sName = cString::Sub(Geometry.m_sName,0,lPos);
			}

			TiXmlElement *pMeshElem = pGeomElem->FirstChildElement("mesh");
			if (pMeshElem==NULL)
			{
				Warning("No Mesh element found in geometry element '%s'!\n", Geometry.m_sName.c_str());
				continue;
			}

			if (cString::Sub(Geometry.m_sName,0,4) == "_ref" ||
				cString::Sub(Geometry.m_sName,0,3) == "_bb" ||
				cString::Sub(Geometry.m_sName,0,4) == "_ps" ||
				cString::Sub(Geometry.m_sName,0,6) == "_sound")
				continue;

			TiXmlElement *pSourceElem = pMeshElem->FirstChildElement("source");
			while (pSourceElem)
			{
				Geometry.m_vArrayVec.push_back(cColladaVtxArray());
				int lPos = (int)Geometry.m_vArrayVec.size()-1;

				tString sSourceId = cString::ToString(pSourceElem->Attribute("id"),"");

				Geometry.m_vArrayVec[lPos].m_sId = sSourceId;

				LoadVertexData(pSourceElem, Geometry.m_vArrayVec[lPos].m_vArray);

				pSourceElem = pSourceElem->NextSiblingElement("source");
			}

			TiXmlElement *pVerticesElem = pMeshElem->FirstChildElement("vertices");
			if (pVerticesElem==NULL)
			{
				Error("Vertices not found!\n");
				return;
			}

			TiXmlElement *pVtxInput = pVerticesElem->FirstChildElement("input");
			while (pVtxInput)
			{
				tString sSemantic = cString::ToString(pVtxInput->Attribute("semantic"),"");

				if (sSemantic == "POSITION")
				{
					tString sSource = cString::ToString(pVtxInput->Attribute("source"),"");
					GetAddress(sSource);

					Log("Position vertex source: %s\n", sSource.c_str());

					for (int i=0;i<(int) Geometry.m_vArrayVec.size();i++)
					{
						if (Geometry.m_vArrayVec[i].m_sId == sSource)
						{
							Geometry.m_vArrayVec[i].m_sId = cString::ToString(pVerticesElem->Attribute("id"),"");
							break;
						}
					}
				}
				else
				{
					tString sSource = cString::ToString(pVtxInput->Attribute("source"),"");
					GetAddress(sSource);

					for (int i=0;i<(int) Geometry.m_vArrayVec.size();i++)
						if (Geometry.m_vArrayVec[i].m_sId == sSource)
						{
							Geometry.m_vArrayVec[i].m_sType = sSemantic;
							Geometry.m_vArrayVec[i].m_bIsInVertex = true;
							break;
						}
				}
				pVtxInput = pVtxInput->NextSiblingElement("input");

			}

			//Get the triangles, save them in a row
			bool bMultiTexture = false;
			TiXmlElement *pTriElem = pMeshElem->FirstChildElement("triangles");
			if (pTriElem == NULL)
			{
				pTriElem = pMeshElem->FirstChildElement("polylist");
				if (pTriElem && pTriElem->NextSibling("polylist")) bMultiTexture = true;
			}
			else
				if (pTriElem->NextSibling("triangles")) bMultiTexture = true;

			if (bMultiTexture)
			{
				cColladaNode *pGeomNode = a_pColladaScene->GetNodeFromSource(Geometry.m_sId);
				tString sParentName = pGeomNode->m_pParent ? pGeomNode->m_pParent->m_sName : "[none]";
				Warning("Geometry '%s' in node '%s' with parent '%s' seems to have multitexturing!\n", Geometry.m_sName.c_str(),
					pGeomNode->m_sName.c_str(), sParentName.c_str());
			}

			if (pTriElem==NULL)
			{
				Warning("No trianlge or polylist element found, testing polygons.\n");
				pTriElem = pMeshElem->FirstChildElement("polygons");
				if (pTriElem==NULL)
				{
					Error("No polygons found!\n");
					return;
				}
			}
			int lTriElements = 0;

			int lTriCount = cString::ToInt(pTriElem->Attribute("count"),0);
			Geometry.m_sMaterial = cString::ToString(pTriElem->Attribute("material"),"");
			GetAddress(Geometry.m_sMaterial);

			TiXmlElement *pTriInputElem = pTriElem->FirstChildElement("input");
			while(pTriInputElem)
			{
				tString sSemantic = cString::ToString(pTriInputElem->Attribute("semantic"),"");
				tString sSource = cString::ToString(pTriInputElem->Attribute("source"),"");
				int lIdx = cString::ToInt(pTriInputElem->Attribute("idx"),-1);
				if (lIdx < 0) lIdx = cString::ToInt(pTriInputElem->Attribute("offset"),-1);

				GetAddress(sSource);

				int lArrayNum = -1;

				for (int i = 0;i < (int)Geometry.m_vArrayVec.size();i++)
				{
					if (Geometry.m_vArrayVec[i].m_sId == sSource)
					{
						lArrayNum = i;
						break;
					}
				}

				if (sSemantic == "VERTEX")
				{
					Geometry.m_lPosIdxNum = lIdx;
					Geometry.m_lPosArrayIdx = lArrayNum;
				}
				else if (sSemantic == "NORMAL")
				{
					Geometry.m_lNormIdxNum = lIdx;
					Geometry.m_lNormArrayIdx = lArrayNum;
				}
				else if (sSemantic == "TEXCOORD")
				{
					Geometry.m_lTexIdxNum = lIdx;
					Geometry.m_lTexArrayIdx = lArrayNum;
				}

				if (lTriElements < lIdx+1) lTriElements = lIdx+1;

				pTriInputElem = pTriInputElem->NextSiblingElement("input");
			}

			for (int i = 0;i < (int)Geometry.m_vArrayVec.size();i++)
			{
				if (Geometry.m_vArrayVec[i].m_bIsInVertex)
				{
					if (Geometry.m_vArrayVec[i].m_sType == "NORMAL")
					{
						Geometry.m_lNormArrayIdx = i;
						Geometry.m_lNormIdxNum = Geometry.m_lPosIdxNum;
					}
					else if (Geometry.m_vArrayVec[i].m_sType == "TEXCOORD")
					{
						Geometry.m_lTexArrayIdx = i;
						Geometry.m_lTexIdxNum = Geometry.m_lPosIdxNum;
					}
				}
			}

			if (Geometry.m_lNormArrayIdx<0)
			{
				Warning("No normals for geometry '%s'\n", Geometry.m_sName.c_str());
				continue;
			}
			bool bHasTexCoords = true;
			if (Geometry.m_lTexArrayIdx<0 && Geometry.m_sName[0] != '_')
			{
				Warning("No tex coords for geometry '%s'\n", Geometry.m_sName.c_str());
				bHasTexCoords = false;
				//continue;
			}

			if (m_bZToY)
			{
				//Positions
				tVector3fVec &vVtxVec = Geometry.m_vArrayVec[Geometry.m_lPosArrayIdx].m_vArray;
				for (int i = 0;i < (int)vVtxVec.size();i++)
					vVtxVec[i] = GetVectorPos(vVtxVec[i]);

				//Normals
				tVector3fVec &vNormVec = Geometry.m_vArrayVec[Geometry.m_lNormArrayIdx].m_vArray;
				for (int i = 0;i < (int)vNormVec.size();i++)
					vNormVec[i] = GetVectorPos(vNormVec[i]);
			}

			Geometry.m_vIndices.reserve(lTriElements * 3);

			g_pVertexVec = &Geometry.m_vArrayVec[Geometry.m_lPosArrayIdx].m_vArray;

			tColladaTestTriMap map_TestTris;

			//Load all the Indices
			TiXmlElement *pPElem = pTriElem->FirstChildElement("p");
			while (pPElem)
			{
				TiXmlText *pText = pPElem->FirstChild()->ToText();
				if (pText==NULL) {Error("No tri data found!\n"); return;}

				//Get the indices for the triangle
				tIntVec vIndexArray;
				cString::GetIntVec(pText->Value(), vIndexArray);

				int lTriangleNum = (int)vIndexArray.size()/(3*lTriElements);
				for (int triangle=0; triangle < lTriangleNum; triangle++)
				{
					cColladaVtxIndex DataVec[3];
					int lTriangleAdd = triangle*3*lTriElements;

					for (int i=0;i<3;i++)
					{
						DataVec[i].m_lVtx = vIndexArray[lTriangleAdd + i*lTriElements+Geometry.m_lPosIdxNum];
						DataVec[i].m_lNorm = vIndexArray[lTriangleAdd + i*lTriElements+Geometry.m_lNormIdxNum];
						// if there's tex coords
						if (bHasTexCoords)
							DataVec[i].m_lTex = vIndexArray[lTriangleAdd + i*lTriElements+Geometry.m_lTexIdxNum];
					}

					std::pair<tColladaTestTriMapIt, bool> TestPair = map_TestTris.insert(DataVec);
					if (TestPair.second)
					{
						//Add the data to the indices
						for (int i=0;i<3;i++)
							Geometry.m_vIndices.push_back(DataVec[i]);
					}
					else
					{
						cColladaNode *pGeomNode = a_pColladaScene->GetNodeFromSource(Geometry.m_sId);
						if (pGeomNode)
						{
							tString sParentName = pGeomNode->m_pParent ? pGeomNode->m_pParent->m_sName : "[none]";
							Warning("Geometry '%s' in node '%s' with parent '%s' has two faces using the same vertices! Skipping face.\n",Geometry.m_sName, pGeomNode->m_sName.c_str(),
								sParentName.c_str());
						}
						else
							Warning("Geometry '%s' has two faces using the same vertices! Skipping face. (note: the geometry node could not be found either!)\n",Geometry.m_sId.c_str());
					}
				}
				pPElem = pPElem->NextSiblingElement("p");
			}

			SplitVertices(Geometry, Geometry.m_vExtraVtxVec, Geometry.m_vVertexVec, Geometry.m_vIndexVec);
			Geometry.Clear();

			tFloatVec vPosVec;	vPosVec.resize(Geometry.m_vVertexVec.size()*4);
			tFloatVec vNormVec;	vNormVec.resize(Geometry.m_vVertexVec.size()*3);
			tFloatVec vTexVec;
			if (bHasTexCoords)
				vTexVec.resize(Geometry.m_vVertexVec.size()*3);

			float *pPosData = &vPosVec[0];
			float *pNormData = &vNormVec[0];
			float *pTexData;
			if (bHasTexCoords)
				pTexData = &vTexVec[0];

			//Fill vectors
			for (size_t i=0;i<Geometry.m_vVertexVec.size();i++)
			{
				cVertex &vertex = Geometry.m_vVertexVec[i];

				pPosData[0] = vertex.pos.x;
				pPosData[1] = vertex.pos.y;
				pPosData[2] = vertex.pos.z;
				pPosData[3] = 1;

				pNormData[0] = vertex.norm.x;
				pNormData[1] = vertex.norm.y;
				pNormData[2] = vertex.norm.z;

				if (bHasTexCoords)
				{
					pTexData[0] = vertex.tex.x;
					pTexData[1] = vertex.tex.y;
					pTexData[2] = vertex.tex.z;
				}

				pPosData += 4;
				pNormData += 3;
				pTexData += 3;
			}

			if (bHasTexCoords)
			{
				Geometry.m_vTangents.resize(Geometry.m_vVertexVec.size()*4);
				cMath::CreateTriTangentVectors(&Geometry.m_vTangents[0],
					&Geometry.m_vIndexVec[0], (int)Geometry.m_vIndexVec.size(),
					&vPosVec[0],4,&vTexVec[0],&vNormVec[0],
					(int)Geometry.m_vVertexVec.size());
			}
		}
	}

	void cMeshLoaderCollada::LoadVertexData(TiXmlElement *a_pSourceElem, tVector3fVec &a_vVtxVec)
	{
		TiXmlElement *pTechniqueElem = a_pSourceElem->FirstChildElement("technique_common");
		if (pTechniqueElem==NULL)pTechniqueElem = a_pSourceElem->FirstChildElement("technique");
		if (pTechniqueElem==NULL)
		{
			Warning("No technique or technique_common element found!\n");
			return;
		}

		TiXmlElement *pAccessor = pTechniqueElem->FirstChildElement("accessor");
		if (pAccessor==NULL)
		{
			Warning("No accessor element found!\n");
			return;
		}

		int lElements = cString::ToInt(pAccessor->Attribute("stride"),0);
		int lVtxCount = cString::ToInt(pAccessor->Attribute("count"),0);

		TiXmlElement *pDataElem = a_pSourceElem->FirstChildElement("float_array");
		if (pDataElem==NULL)
		{
			pDataElem = a_pSourceElem->FirstChildElement("array");
			if (pDataElem)
			{
				Warning("No data found!\n");
				return;
			}
		}

		TiXmlText *pTextElem = pDataElem->FirstChild()->ToText();
		if (pTextElem==NULL)
		{
			Warning("No text found!\n");
			return;
		}
		const char *pChars = pTextElem->Value();

		FillVertexVec(pChars, a_vVtxVec, lElements, lVtxCount);
	}

	static cColladaAnimation &GetAnimationFromTarget(const tString &a_sTargetNode, tColladaAnimationVec &a_vAnimations)
	{
		for (size_t i=0; i<a_vAnimations.size(); i++)
		{
			if (a_vAnimations[i].m_sTargetNode == a_sTargetNode)
				return a_vAnimations[i];
		}

		a_vAnimations.push_back(cColladaAnimation());
		cColladaAnimation &Anim = a_vAnimations[a_vAnimations.size()-1];
		Anim.m_sTargetNode = a_sTargetNode;

		return Anim;
	}

	void cMeshLoaderCollada::LoadAnimations(TiXmlElement *a_pRootElem, tColladaAnimationVec &a_vAnimations,
		cColladaScene *a_pColladaScene)
	{
		TiXmlElement *pAnimElem = a_pRootElem->FirstChildElement("animation");
		for (; pAnimElem != NULL; pAnimElem = pAnimElem->NextSiblingElement("animation"))
		{
			TiXmlElement *pTestChannelElem = pAnimElem->FirstChildElement("channel");
			if (pTestChannelElem == NULL) {Warning("Animation missing channel!\n"); continue;}

			// Get target node name
			tString sTestTarget = cString::ToString(pTestChannelElem->Attribute("target"), "");
			tStringVec vTargertStrings;
			tString stargetSepp = "/";
			cString::GetStringVec(sTestTarget, vTargertStrings, &stargetSepp);
			sTestTarget = vTargertStrings[0];

			cColladaAnimation &Anim = GetAnimationFromTarget(sTestTarget, a_vAnimations);

			Anim.m_sId = cString::ToString(pAnimElem->Attribute("id"),"");

			// Load all Channels
			TiXmlElement *pChannelElem = pAnimElem->FirstChildElement("channel");
			for (; pChannelElem != NULL; pChannelElem = pChannelElem->NextSiblingElement("channel"))
			{
				cColladaChannel Channel;
				Channel.m_sId = cString::ToString(pChannelElem->Attribute("id"),"");
				Channel.m_sSource = cString::ToString(pChannelElem->Attribute("source"),"");
				Channel.m_sTarget = cString::ToString(pChannelElem->Attribute("target"),"");
				GetAddress(Channel.m_sSource);

				Anim.m_vChannels.push_back(Channel);
			}

			// Load all Samplers
			TiXmlElement *pSamplerElem = pAnimElem->FirstChildElement("sampler");
			for (; pSamplerElem != NULL; pSamplerElem = pSamplerElem->NextSiblingElement("sampler"))
			{
				cColladaSampler Sampler;
				Sampler.m_sId = cString::ToString(pSamplerElem->Attribute("id"),"");

				// Iterate the inputs and find the needed types
				TiXmlElement *pInput = pSamplerElem->FirstChildElement("input");
				for (; pInput != NULL; pInput = pInput->NextSiblingElement("input"))
				{
					tString sSemantic = cString::ToString(pInput->Attribute("semantic"),"");
					tString sSource = cString::ToString(pInput->Attribute("source"),"");
					GetAddress(sSource);

					if (sSemantic == "INPUT")
						Sampler.m_sTimeArray = sSource;
					else if (sSemantic == "OUTPUT")
						Sampler.m_sValueArray = sSource;
					else if (sSemantic == "INTERPOLATION")
						Sampler.m_sInterpolationArray = sSource;
					else if (sSemantic == "IN_TANGENT")
						Sampler.m_sInTangentArray = sSource;
					else if (sSemantic == "OUT_TANGENT")
						Sampler.m_sOutTangentArray = sSource;

				}
				Anim.m_vSamplers.push_back(Sampler);
			}

			for (size_t i = 0; i < Anim.m_vChannels.size(); i++)
			{
				for (size_t j = 0; j < Anim.m_vSamplers.size(); j++)
				{
					if (Anim.m_vChannels[i].m_sSource == Anim.m_vSamplers[j].m_sId)
						Anim.m_vSamplers[j].m_sTarget = Anim.m_vChannels[i].m_sTarget;
				}
			}

			TiXmlElement *pSourceElem = pAnimElem->FirstChildElement("source");
			for (; pSourceElem != NULL; pSourceElem = pSourceElem->NextSiblingElement("source"))
			{
				Anim.m_vSources.push_back(cColladaAnimSource());
				cColladaAnimSource &Source = Anim.m_vSources[Anim.m_vSources.size() - 1];

				Source.m_sId = cString::ToString(pSourceElem->Attribute("id"),"");

				bool bStrings = false;
				TiXmlElement *pArrayElem = pSourceElem->FirstChildElement("float_array");
				if (pArrayElem == NULL)
				{
					pArrayElem = pSourceElem->FirstChildElement("Name_array");
					bStrings = true;
					if (pArrayElem == NULL)
						continue;
				}

				int lCount = cString::ToInt(pArrayElem->Attribute("count"), 0);

				if (!bStrings)
					Source.m_vValues.reserve(lCount);
				else
					Source.m_vNameValues.reserve(lCount);

				TiXmlText *pText = pArrayElem->FirstChild()->ToText();

				if (!bStrings)
					cString::GetFloatVec(pText->Value(), Source.m_vValues);
				else
					cString::GetStringVec(pText->Value(), Source.m_vNameValues);
			}
		}
	}

	//--------------------------------------------------------------

	void cMeshLoaderCollada::LoadImages(TiXmlElement *a_pRootElem, tColladaImageVec &a_vColladaImageVec)
	{
		TiXmlElement *pImageElem = a_pRootElem->FirstChildElement("image");
		while (pImageElem)
		{
			cColladaImage Image;
			Image.m_sId = cString::ToString(pImageElem->Attribute("id"),"");
			Image.m_sName = cString::ToString(pImageElem->Attribute("name"),"");

			TiXmlElement *pInitFromElem = pImageElem->FirstChildElement("init_from");

			if (pInitFromElem)
			{
				if (pInitFromElem->FirstChild())
				{
					TiXmlText *pText = pInitFromElem->FirstChild()->ToText();
					Image.m_sSource = cString::ToString(pText->Value(),"");
				}
				else
					Image.m_sSource = "";
			}
			else
				Image.m_sSource = cString::ToString(pImageElem->Attribute("source"),"");

			a_vColladaImageVec.push_back(Image);

			pImageElem = pImageElem->NextSiblingElement("image");
		}
	}

	//--------------------------------------------------------------

	class cEffectNewParam
	{
	public:
		tString m_sId;
		tString m_sType;
		tString m_sData;
	};

	tString *GetFinalSource(std::vector<cEffectNewParam> &a_vParams, tString &a_sId)
	{
		for (size_t i=0; i<a_vParams.size();++i)
		{
			if (a_sId == a_vParams[i].m_sId)
				return GetFinalSource(a_vParams, a_vParams[i].m_sData);
		}
		return &a_sId;
	}

	void cMeshLoaderCollada::LoadTextures(TiXmlElement *a_pRootElem, tColladaTextureVec &a_vColladaTextureVec)
	{
		TiXmlElement *pTextureElem = a_pRootElem->FirstChildElement();
		for (;pTextureElem != NULL;pTextureElem = pTextureElem->NextSiblingElement())
		{
			cColladaTexture Texture;

			Texture.m_sId = cString::ToString(pTextureElem->Attribute("id"),"");

			TiXmlElement *pProfileCommon = pTextureElem->FirstChildElement("profile_COMMON");
			//COLLADA 1.4
			if (pProfileCommon)
			{
				std::vector<cEffectNewParam> vNewParams;
				TiXmlElement *pNewParamElem = pProfileCommon->FirstChildElement("newparam");
				for (;pNewParamElem != NULL;pNewParamElem = pNewParamElem->NextSiblingElement("newparam"))
				{
					vNewParams.push_back(cEffectNewParam());
					cEffectNewParam &newParam = vNewParams.back();

					newParam.m_sId = pNewParamElem->Attribute("sid");

					TiXmlElement *pChildElem = pNewParamElem->FirstChildElement();
					if (pChildElem)
					{
						newParam.m_sType = pChildElem->Value();

						tString sDataName = "";
						if (newParam.m_sType == "surface")			sDataName = "init_from";
						else if (newParam.m_sType == "sampler2D")	sDataName = "source";

						if (sDataName == "") continue;
						TiXmlElement *pValueElem = pChildElem->FirstChildElement(sDataName.c_str());
						if (pValueElem)
						{
							TiXmlText *pText = pValueElem->FirstChild()->ToText();
							newParam.m_sData = pText->Value();
						}
						else
							Warning("Data element '%s' missing from newparam '%s'\n", sDataName.c_str(),
							newParam.m_sId.c_str());
					}
				}

				TiXmlElement *pTechniqueElem = pProfileCommon->FirstChildElement("technique");
				if (pTechniqueElem==NULL)
				{
					Warning("No effect technique element found!\n");
					continue;
				}

				TiXmlElement *pTypeElem = pTechniqueElem->FirstChildElement();
				if (pTypeElem==NULL)
				{
					Warning("No effect type element found!\n");
					continue;
				}
				if (cString::ToString(pTypeElem->Value(),"") == "extra")
				{
					pTypeElem = pTypeElem->NextSiblingElement();
					if (pTypeElem==NULL)
					{
						Warning("No effect type element found!\n");
						continue;
					}
				}

				TiXmlElement *pDiffuseElem = pTypeElem->FirstChildElement("diffuse");
				if (pDiffuseElem)
				{
					TiXmlElement *pLocalTexture = pDiffuseElem->FirstChildElement("texture");
					if (pLocalTexture)
					{
						tString _tstr = cString::ToString(pLocalTexture->Attribute("texture"),"");
						Texture.m_sImage = *GetFinalSource(vNewParams, _tstr);
					}
					else
					{
						Texture.m_sImage = "";
						Warning("No diffuse texture effect element for effect '%s'! No file texture will be loaded.\n",
							Texture.m_sId.c_str());
					}
				}
				else
					Warning("No diffuse effect element!\n");
			}
			// COLLADA 1.3
			else
			{
				TiXmlElement *pParamElem = pTextureElem->FirstChildElement("param");
				while (pParamElem)
				{
					pParamElem = pParamElem->NextSiblingElement("param");
				}

				TiXmlElement *pTechniqueElem = pTextureElem->FirstChildElement("technique");
				while (pTechniqueElem)
				{
					tString sProfile = cString::ToString(pTechniqueElem->Attribute("profile"),"");

					TiXmlElement *pTechParam = pTechniqueElem->FirstChildElement("param");
					while(pTechParam)
					{
						pTechParam = pTechParam->NextSiblingElement("param");
					}

					TiXmlElement *pTechInput = pTechniqueElem->FirstChildElement("input");
					while (pTechInput)
					{
						tString sSemantic = cString::ToString(pTechInput->Attribute("semantic"),"");
						if (sSemantic == "IMAGE")
							Texture.m_sImage = cString::ToString(pTechInput->Attribute("source"),"");

						pTechInput = pTechInput->NextSiblingElement("input");
					}

					pTechniqueElem = pTechniqueElem->NextSiblingElement("technique");
				}
			}
			a_vColladaTextureVec.push_back(Texture);
		}
	}

	//--------------------------------------------------------------

	void cMeshLoaderCollada::LoadMaterials(TiXmlElement *a_pRootElem, tColladaMaterialVec &a_vColladaMaterialVec)
	{
		TiXmlElement *pMaterialElem = a_pRootElem->FirstChildElement("material");
		while (pMaterialElem)
		{
			cColladaMaterial Material;
			Material.m_sId = cString::ToString(pMaterialElem->Attribute("id"),"");
			Material.m_sName = cString::ToString(pMaterialElem->Attribute("name"),"");

			TiXmlElement *pInstanceEffectElem = pMaterialElem->FirstChildElement("instance_effect");
			if (pInstanceEffectElem)
				Material.m_sTexture = cString::ToString(pInstanceEffectElem->Attribute("url"),"");
			else
			{
				TiXmlElement *pShaderElem = pMaterialElem->FirstChildElement("shader");
				if (pShaderElem == NULL) 
				{
					Warning("No shader found!\n");
					continue;
				}

				TiXmlElement *pTechElem = pShaderElem->FirstChildElement("technique");
				if (pTechElem == NULL) 
				{
					Warning("No technique found!\n");
					continue;
				}

				TiXmlElement *pPassElem = pTechElem->FirstChildElement("pass");
				if (pPassElem == NULL) 
				{
					Warning("No pass found!\n");
					continue;
				}

				TiXmlElement *pInputElem = pPassElem->FirstChildElement("input");
				while (pInputElem)
				{
					tString sSemantic = cString::ToString(pInputElem->Attribute("semantic"),"");

					if (sSemantic == "TEXTURE")
						Material.m_sTexture = cString::ToString(pInputElem->Attribute("source"),"");

					pInputElem = pInputElem->NextSiblingElement();
				}
			}
			//Log("Material: id: '%s' name: '%s' texture: '%s'\n", Material.m_sId.c_str(),
			//													Material.m_sName.c_str(),
			//													Material.m_sTexture.c_str());

			a_vColladaMaterialVec.push_back(Material);

			pMaterialElem = pMaterialElem->NextSiblingElement("material");
		}

	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// HELPER METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	static cVertex IndexDataToVertex(const cColladaVtxIndex &a_Data, const cColladaGeometry &a_Geometry)
	{
		cVertex Vtx;

		Vtx.col = cColor(1,1);
		Vtx.pos = a_Geometry.m_vArrayVec[a_Geometry.m_lPosArrayIdx].m_vArray[a_Data.m_lVtx];
		if (a_Geometry.m_lNormArrayIdx>=0)
			Vtx.norm = a_Geometry.m_vArrayVec[a_Geometry.m_lNormArrayIdx].m_vArray[a_Data.m_lNorm];
		if (a_Geometry.m_lTexArrayIdx>=0)
			Vtx.tex = a_Geometry.m_vArrayVec[a_Geometry.m_lTexArrayIdx].m_vArray[a_Data.m_lTex];

		Vtx.tex.y = 1-Vtx.tex.y;

		return Vtx;
	}

	static cColladaExtraVtx IndexDataToExtra(const cColladaVtxIndex &a_Data, int a_lNewVtx)
	{
		cColladaExtraVtx Extra;
		Extra.m_lNewVtx = a_lNewVtx;
		Extra.m_lVtx = a_Data.m_lVtx;
		Extra.m_lNorm = a_Data.m_lNorm;
		Extra.m_lTex = a_Data.m_lTex;
		return Extra;
	}

	void cMeshLoaderCollada::SplitVertices(cColladaGeometry &a_Geometry, tColladaExtraVtxListVec &a_vExtraVtxVec,
		tVertexVec &a_vVertexVec, tUIntVec &a_vIndexVec)
	{
		//Resize the extra array and the vertex array
		int lVtxSize = (int)a_Geometry.m_vArrayVec[a_Geometry.m_lPosArrayIdx].m_vArray.size();
		a_vExtraVtxVec.resize(lVtxSize);
		a_vVertexVec.resize(lVtxSize);
		a_vIndexVec.resize(a_Geometry.m_vIndices.size());

		tColladaVtxIndexVec &vIndices = a_Geometry.m_vIndices;

		for (int i=0;i<(int)vIndices.size();i++)
		{
			cColladaVtxIndex &Data = vIndices[i];
			//Log("Index %d: ",i);

			if (a_vExtraVtxVec[Data.m_lVtx].empty())
			{
				a_vExtraVtxVec[Data.m_lVtx].push_back(IndexDataToExtra(Data, Data.m_lVtx));

				a_vVertexVec[Data.m_lVtx] = IndexDataToVertex(Data, a_Geometry);
				a_vIndexVec[i] = Data.m_lVtx;
			}
			else
			{
				bool bFound = false;
				tColladaExtraVtxListIt it = a_vExtraVtxVec[Data.m_lVtx].begin();
				for (;it!=a_vExtraVtxVec[Data.m_lVtx].end();++it)
				{
					if (it->Equals(Data))
					{
						a_vIndexVec[i] = it->m_lNewVtx;
						bFound = true;
						//Log("Old vertex split used");
						break;
					}
				}

				if (!bFound)
				{
					a_vExtraVtxVec[Data.m_lVtx].push_back(IndexDataToExtra(Data, (int)a_vVertexVec.size()));
					a_vIndexVec[i] = (int)a_vVertexVec.size();

					a_vVertexVec.push_back(IndexDataToVertex(Data, a_Geometry));
				}
			}
		}
	}

	//--------------------------------------------------------------

	void cMeshLoaderCollada::FillVertexVec(const char *a_pChars, tVector3fVec &a_vVtxVec, int a_lElements, int a_lVtxCount)
	{
		if ((int)a_vVtxVec.size() < a_lVtxCount) a_vVtxVec.resize(a_lVtxCount);
		int lArraySize = a_lElements * a_lVtxCount;

		int lArrayCount = 0;
		int lStringCount = 0;

		float vValArray[3];
		int lVarArrayCount = 0;
		char vTempChar[20];
		int lTempCharCount = 0;

		int lVertexNum = 0;

		while(lArrayCount < lArraySize)
		{
			char c = a_pChars[lStringCount];
			if (c == ' ' || c == 0 || c=='\n' || c== '\t')
			{
				if (lTempCharCount>0)
				{
					vTempChar[lTempCharCount] = 0;
					vValArray[lVarArrayCount] = (float)atof(vTempChar);

					lTempCharCount = 0;
					lArrayCount++;

					lVarArrayCount++;
					if (lVarArrayCount == a_lElements)
					{
						lVarArrayCount = 0;
						cVector3f vVec;
						vVec.x = vValArray[0];vVec.y = vValArray[1];
						if (a_lElements == 3) vVec.z = vValArray[2];

						a_vVtxVec[lVertexNum] = vVec;
						lVertexNum++;
					}
				}
			}
			else
			{
				vTempChar[lTempCharCount] = a_pChars[lStringCount];
				lTempCharCount++;
			}

			lStringCount++;
		}
	}

	//--------------------------------------------------------------

	tString cMeshLoaderCollada::GetMaterialTextureFile(const tString &a_sMaterial, tColladaMaterialVec &a_vColladaMaterialVec,
		tColladaTextureVec &a_vColladaTextureVec,
		tColladaImageVec &a_vColladaImageVec)
	{
		tString sTexId = "";
		for (size_t mat=0; mat<a_vColladaMaterialVec.size(); mat++)
		{
			if (a_vColladaMaterialVec[mat].m_sId == a_sMaterial)
			{
				sTexId = a_vColladaMaterialVec[mat].m_sTexture;
				GetAddress(sTexId);
				break;
			}
		}

		if (sTexId == "")
		{
			Warning("Material '%s' was not found!\n", a_sMaterial.c_str());
			return "";
		}

		tString sFileId = "";
		for (size_t tex=0; tex<a_vColladaTextureVec.size(); tex++)
		{
			if (a_vColladaTextureVec[tex].m_sId == sTexId)
			{
				sFileId = a_vColladaTextureVec[tex].m_sImage;
				GetAddress(sFileId);
				break;
			}
		}

		if (sFileId == "")
		{
			Warning("Texture '%s' was not found!\n", sTexId.c_str());
			return "";
		}

		for (size_t img=0; img<a_vColladaImageVec.size(); img++)
		{
			if (a_vColladaImageVec[img].m_sId == sFileId)
				return cString::GetFileName(a_vColladaImageVec[img].m_sSource);
		}

		Warning("Couldn't load file image file id '%s'\n", sFileId.c_str());

		return "";
	}
}