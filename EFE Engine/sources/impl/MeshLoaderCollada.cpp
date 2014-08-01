#include "impl/MeshLoaderCollada.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "system/String.h"
#include "system/System.h"

#include "scene/Scene.h"
#include "scene/PortalContainer.h"
#include "scene/World3D.h"
#include "scene/MeshEntity.h"
#include "scene/Light3DPoint.h"

#include "scene/Node3D.h"
#include "scene/ColliderEntity.h"
#include "scene/SoundEntity.h"

#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/Material.h"

#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/Resources.h"
#include "resources/FileSearcher.h"

#include "graphics/Bone.h"
#include "graphics/Skeleton.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"

#include "physics/Physics.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsMaterial.h"
#include "physics/SurfaceData.h"

#include "haptic/Haptic.h"


#include "haptic/LowLevelHaptic.h"

#include "impl/tinyXML/tinyxml.h"

#include "math/Math.h"

namespace efe
{
#define GetAddress(sStr) if (sStr[0]=='#') sStr = cString::Sub(sStr, 1);

	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cMeshLoaderCollada::cMeshLoaderCollada(iLowLevelGraphics *a_pLowLevelGraphics)
		: iMeshLoader(a_pLowLevelGraphics)
	{
		m_Flags = 0;
	}

	//--------------------------------------------------------------

	cMeshLoaderCollada::~cMeshLoaderCollada()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cWorld3D *cMeshLoaderCollada::LoadWorld(const tString &a_sFile, cScene *a_pScene, tWorldLoadFlag a_Flags)
	{
		tColladaImageVec vColladaImages;

		tColladaTextureVec vColladaTextures;

		tColladaMaterialVec vColladaMaterials;

		tColladaLightVec vColladaLights;

		tColladaGeometryVec vColladaGeometries;

		cColladaScene ColladaScene;

		m_Flags = a_Flags;

		unsigned long lStartTime = GetApplicationTime();

		bool bRet = FillStructures(a_sFile, &vColladaImages, &vColladaTextures,
										&vColladaMaterials, &vColladaLights,
										&vColladaGeometries,
										NULL, NULL,
										&ColladaScene, true);

		unsigned long lTime = GetApplicationTime() - lStartTime;
		Log("Loading collada for '%s' took: %lu ms\n", a_sFile.c_str(), lTime);

		if (bRet == false) return NULL;

		cWorld3D *pWorld = a_pScene->CreateWorld3D(cString::SetFileExt(cString::GetFileName(a_sFile), ""));
		pWorld->SetFileName(cString::GetFileName(a_sFile));

		cPortalContainer *pPortalContainer = pWorld->GetPortalContainer();

		//Create the physics world
		iPhysicsWorld *pPhysicsWorld = pWorld->GetPhysics()->CreateWorld(true);
		pWorld->SetPhysicsWorld(pPhysicsWorld, true);

		tColladaNodeListIt it = ColladaScene.m_Root.m_lstChildren.begin();
		for (; it != ColladaScene.m_Root.m_lstChildren.end(); it++)
		{
			AddSceneObjects(*it, pWorld, vColladaGeometries, vColladaLights,
				vColladaMaterials, vColladaTextures, vColladaImages, &ColladaScene);
		}

		pWorld->SetUpData();

		return pWorld;
	}

	static cColladaNode *GetNodeFromController(const tString &a_sGeomId,
												tColladaControllerVec &a_vColladaControllers,
												cColladaScene &a_ColladaScene)
	{
		tString sControlId = "";
		bool bGuess = false;
		for (int ctrl=0;ctrl<(int)a_vColladaControllers.size();ctrl++)
		{
			cColladaController &Control = a_vColladaControllers[ctrl];
			if (Control.m_sId == a_sGeomId)
			{
				sControlId = Control.m_sId;
				bGuess = false;
			}
			else if (sControlId=="" && Control.m_sTarget=="")
			{
				sControlId = Control.m_sId;
				bGuess = true;
			}
		}
		if (bGuess) Warning("No controller for geometry '%s', guessing on '%s' target = ''\n",
							a_sGeomId.c_str(), sControlId.c_str());

		if (sControlId == "")
		{
			Warning("No controller refered to the geometry!\n");
			return NULL;
		}

		cColladaNode *pNode = a_ColladaScene.GetNodeFromSource(sControlId);
		if (pNode==NULL)
			Warning("No node for controller '%s'\n", sControlId.c_str());
		return pNode;
	}

	cMesh *cMeshLoaderCollada::LoadMesh(const tString &a_sFile, tMeshLoadFlag a_Flags)
	{
		tColladaImageVec vColladaImages;

		tColladaTextureVec vColladaTextures;

		tColladaMaterialVec vColladaMaterials;

		tColladaLightVec vColladaLights;

		tColladaGeometryVec vColladaGeometries;

		tColladaControllerVec vColladaControllers;

		tColladaAnimationVec vColladaAnimations;

		cColladaScene ColladaScene;

		m_Flags = a_Flags;

		tColladaGeometryVec *pGeomVec = (a_Flags & eMeshLoadFlag_NoGeometry) ? NULL : &vColladaGeometries;

		bool bRet = FillStructures(a_sFile, &vColladaImages, &vColladaTextures,
										&vColladaMaterials, &vColladaLights,
										&vColladaGeometries, &vColladaControllers,
										&vColladaAnimations,
										&ColladaScene, true);

		if (!bRet) return NULL;

		cSkeleton *pSkeleton = NULL;

		//Check for joints or several bodies
		bool bHasSeveralBodies = false;

		tString sCollideGroup = "";
		bool bFoundCollider = false;
		tColladaNodeListIt nodeIt = ColladaScene.m_lstNodes.begin();
		for (; nodeIt != ColladaScene.m_lstNodes.end(); nodeIt++)
		{
			cColladaNode *pNode = *nodeIt;

			if (cString::ToLowerCase(cString::Sub(pNode->m_sName,0,6)) == "_joint")
			{
				bHasSeveralBodies = true;
				break;
			}

			if (cString::ToLowerCase(cString::Sub(pNode->m_sName,0,9)) == "_collider")
			{
				tString sGroup = "";
				if (pNode->m_pParent) sGroup = pNode->m_pParent->m_sName;

				if (bFoundCollider==false)
				{
					bFoundCollider = true;
					sCollideGroup = sGroup;
				}
				else if (sGroup != sCollideGroup)
				{
					bHasSeveralBodies = true;

					break;
				}
			}
		}

		//Create Mesh
		tString sMeshName = cString::GetFileName(a_sFile);
		cMesh *pMesh = efeNew(cMesh, (sMeshName, m_pMaterialManager, m_pAnimationManager));

		if (pSkeleton) pMesh->SetSkeleton(pSkeleton);

		//Create Sub meshes
		for (int i=0;i<(int)vColladaGeometries.size();i++)
		{
			cColladaGeometry &Geom = vColladaGeometries[i];

			cColladaNode *pGeomNode = ColladaScene.GetNodeFromSource(Geom.m_sId);
			if (pGeomNode == NULL)
			{
				pGeomNode = GetNodeFromController(Geom.m_sId, vColladaControllers, ColladaScene);
				if (pGeomNode==NULL)
				{
					Error("No node with geometry id '%s'\n", Geom.m_sId.c_str());
					continue;
				}
			}
			tString sNodeName = pGeomNode->m_sName;

			if (sNodeName[0] == '_')
			{
				tStringVec vStrings;
				tString sSepp = "_";
				cString::GetStringVec(sNodeName, vStrings, &sSepp);

				continue;
			}

			tString sSubMeshName = Geom.m_sName==""? Geom.m_sId : Geom.m_sName;
			cSubMesh *pSubMesh = pMesh->CreateSubMesh(sSubMeshName);

			cColladaNode *pNode = ColladaScene.GetNodeFromSource(Geom.m_sId);
			if (pNode==NULL)
			{
				pNode = GetNodeFromController(Geom.m_sId, vColladaControllers, ColladaScene);
				if (pNode == NULL)
				{
					Warning("No node for geometry '%s' found when trying to create sub mesh, check in controllers.\n", Geom.m_sId.c_str());
					continue;
				}
			}

			pSubMesh->SetNodeName(pNode->m_sName);
			pSubMesh->SetLocalTransform(pNode->m_mtxTransform);
			if (pNode->m_pParent)
				pSubMesh->SetGroup(pNode->m_pParent->m_sName);

			//Set the scale
			pSubMesh->SetModelScale(pNode->m_vScale);

			tColladaExtraVtxListVec &vExtraVtxVec = Geom.m_vExtraVtxVec;

			eVertexBufferUsageType UsageType = eVertexBufferUsageType_Static;

			iVertexBuffer *pVtxBuffer = CreateVertexBuffer(Geom, UsageType);
			pSubMesh->SetVertexBuffer(pVtxBuffer);

			tString sMatName = GetMaterialTextureFile(Geom.m_sMaterial, vColladaMaterials, vColladaTextures,
				vColladaImages);

			iMaterial *pMaterial;
			if (m_bUseFastMaterial && m_sFastMaterialFile != "")
				pMaterial = m_pMaterialManager->CreateMaterial(m_sFastMaterialFile);
			else
				pMaterial = m_pMaterialManager->CreateMaterial(sMatName);

			if (pMaterial == NULL)
			{
				Error("Couldn't create material '%s' for object '%s'\n",sMatName.c_str(),
																		Geom.m_sName.c_str());

				continue;
			}
			pSubMesh->SetMaterial(pMaterial);

			cColladaController *pCtrl = NULL;
			for (size_t j=0; j<vColladaControllers.size(); j++)
			{
				if (vColladaControllers[j].m_sTarget == Geom.m_sId)
				{
					pCtrl = &vColladaControllers[j];
					break;
				}
			}

			if (pCtrl==NULL)
			{
				for (size_t j=0; j<vColladaControllers.size(); j++)
				{
					if (vColladaControllers[j].m_sTarget == Geom.m_sId)
					{
						pCtrl = &vColladaControllers[j];
						Warning("Guessing controller as target=''!\n");
						break;
					}
				}
			}

			if (pCtrl && pSkeleton)
			{
			}
			else
			{
				if (vColladaAnimations.empty() == false || bHasSeveralBodies)
				{
					cMatrixf mtxScale = cMath::MatrixScale(pNode->m_vScale);
					pVtxBuffer->Transform(mtxScale);
				}
				else if (bHasSeveralBodies==false)
				{
					cColladaNode *ptNode = ColladaScene.GetNodeFromSource(Geom.m_sId);
					if (ptNode)
						pVtxBuffer->Transform(ptNode->m_mtxTransform);
					else
						Warning("No node for geometry '%s' found when trying to transform geometry!\n", Geom.m_sId.c_str());
				}
			}

			cMath::CreateTriangleData(*pSubMesh->GetTriangleVecPtr(),
				pVtxBuffer->GetIndices(),pVtxBuffer->GetIndexNum(),
				pVtxBuffer->GetArray(eVertexFlag_Position),
				kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)],
				pVtxBuffer->GetVertexNum());

			//Create edges for geometry
			bool bDoubleSided = false;
			cMath::CreateEdges(*pSubMesh->GetEdgeVecPtr(),
								pVtxBuffer->GetIndices(),pVtxBuffer->GetIndexNum(),
								pVtxBuffer->GetArray(eVertexFlag_Position),
								kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)],
								pVtxBuffer->GetVertexNum(),
								&bDoubleSided);
			pSubMesh->SetDoubleSided(bDoubleSided);

			pSubMesh->Compile();
		}

		// Add Node Hierarchy
		if ((vColladaAnimations.empty() == false || bHasSeveralBodies) && !pSkeleton)
		{
			cNode3D *pRootNode = pMesh->GetRootNode();

			tColladaNodeListIt it = ColladaScene.m_Root.m_lstChildren.begin();
			for (; it != ColladaScene.m_Root.m_lstChildren.end(); ++it)
			{
				CreateHierarchyNodes(pMesh, pRootNode, *it, vColladaGeometries);
			}

			if (bHasSeveralBodies)
			{
				for (int i=0; i<pMesh->GetNodeNum(); i++)
				{
					cNode3D *pMeshNode = pMesh->GetNode(i);
					cMatrixf mtxNode = pMeshNode->GetLocalMatrix();

					cSubMesh *pSubMesh = pMesh->GetSubMeshName(pMeshNode->GetSource());

					if (pSubMesh)
					{
						cMatrixf mtxScale = cMath::MatrixScale(pSubMesh->GetModelScale());
						mtxNode = cMath::MatrixMul(mtxNode, cMath::MatrixInverse(mtxScale));
					}
					else if (tString(pMeshNode->GetSource()) != ""
						&& pMeshNode->GetSource()[0] != '_')
						Error("Cannot find submesh '%s'\n", pMeshNode->GetSource());

					pMeshNode->SetMatrix(mtxNode);
				}
			}
		}

		// Create Animations
		cAnimation *pAnimation = NULL;
		if (vColladaAnimations.empty() == false)
		{
			pAnimation = efeNew(cAnimation, (pMesh->GetName(), cString::GetFileName(a_sFile)));

			pAnimation->SetAnimationName("Default");
			pAnimation->SetLength(ColladaScene.m_fDeltaTime);
			pAnimation->ResizeTracks((int)vColladaAnimations.size());

			pMesh->AddAnimation(pAnimation);

			for (size_t i=0; i<vColladaAnimations.size(); i++)
			{
				cAnimationTrack *pTrack = CreateAnimTrack(pAnimation, pSkeleton,
					vColladaAnimations[i], &ColladaScene);
				if (pTrack == NULL) continue;
				if (pSkeleton)
				{
					cBone *pBone = pSkeleton->GetBoneByName(pTrack->GetName());
					int lBoneIdx = pSkeleton->GetBoneIndexByName(pTrack->GetName());
					pTrack->SetNodeIndex(lBoneIdx);
				}
				else
					pTrack->SetNodeIndex(-1);
			}

			if (pSkeleton)
			{
				for (int i=0; i<pSkeleton->GetBoneNum(); ++i)
				{
					cBone *pBone = pSkeleton->GetBoneByIndex(i);

					if (pAnimation->GetTrackByName(pBone->GetName()) == NULL)
					{
						cColladaNode *pNode = ColladaScene.GetNode(pBone->GetName());
						if (pNode)
						{
							cMatrixf mtxLocal = pNode->m_mtxTransform;
							cMatrixf mtxInvBone = cMath::MatrixInverse(pBone->GetLocalTransform());

							cMatrixf mtxChange = cMath::MatrixMul(mtxInvBone, mtxLocal);

							cAnimationTrack *pTrack = pAnimation->CreateTrack(pBone->GetName(), 
								eAnimTransformFlag_Rotate |
								eAnimTransformFlag_Translate |
								eAnimTransformFlag_Scale);
							pTrack->SetNodeIndex(i);
							cKeyFrame *pFrame = pTrack->CreateKeyFrame(0);

							pFrame->trans = mtxChange.GetTranslation();
							pFrame->scale = 1;
							pFrame->rotation = cQuaternion::Identity;

							mtxChange.SetTranslation(0);
							pFrame->rotation.FromRotationMatrix(mtxChange);
						}
						else
							Warning("Couldn't find node for bone '%s'\n", pBone->GetName().c_str()); 
					}
				}
			}
		}

		return pMesh;
	}

	//--------------------------------------------------------------

	bool cMeshLoaderCollada::IsSupported(const tString a_sFileType)
	{
		if (a_sFileType == "dae") return true;

		return false;
	}

	//--------------------------------------------------------------

	void cMeshLoaderCollada::AddSupportedTypes(tStringVec *a_vFileTypes)
	{
		a_vFileTypes->push_back("dae");
	}

	//--------------------------------------------------------------

	static bool HasParam(const tStringVec &a_vVec, const tString &a_sParam)
	{
		for (int i=0; i<(int)a_vVec.size(); i++)
		{
			if (cString::ToLowerCase(a_vVec[i]) == a_sParam)
				return true;
		}

		return false;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void cMeshLoaderCollada::CreateHierarchyNodes(cMesh *a_pMesh, cNode3D *a_pParentNode,
			cColladaNode *a_pColladaNode,
			tColladaGeometryVec &a_vColladaGeom)
	{
		cNode3D *pNode = a_pParentNode->CreateChild3D(a_pColladaNode->m_sName);
		a_pMesh->AddNode(pNode);

		pNode->SetMatrix(a_pColladaNode->m_mtxTransform);

		pNode->SetPosition(pNode->GetLocalPosition());

		if (a_pColladaNode->m_sSource != "")
		{
			for (int i=0; i<(int)a_vColladaGeom.size(); i++)
			{
				if (a_vColladaGeom[i].m_sId == a_pColladaNode->m_sSource)
				{
					pNode->SetSource(a_vColladaGeom[i].m_sName);
					break;
				}
			}
		}

		tColladaNodeListIt it = a_pColladaNode->m_lstChildren.begin();
		for (; it != a_pColladaNode->m_lstChildren.end(); ++it)
			CreateHierarchyNodes(a_pMesh, pNode, *it, a_vColladaGeom);
	}

	//--------------------------------------------------------------

	cColladaGeometry *cMeshLoaderCollada::GetGeometry(const tString &a_sId, tColladaGeometryVec &a_vGeomVec)
	{
		for (size_t i = 0; i < a_vGeomVec.size(); i++)
		{
			if (a_vGeomVec[i].m_sId == a_sId)
				return &a_vGeomVec[i];
		}

		return NULL;
	}

	//--------------------------------------------------------------

	cColladaLight *cMeshLoaderCollada::GetLight(const tString &a_sId, tColladaLightVec &a_vLightVec)
	{
		for (size_t i = 0; i < a_vLightVec.size(); i++)
		{
			if (a_vLightVec[i].m_sId == a_sId)
				return &a_vLightVec[i];
		}

		return NULL;
	}

	//--------------------------------------------------------------

	static iCollideShape *CreatePhysicsCollider(iPhysicsWorld *a_pPhysicsWorld, const cVector3f &a_vSize,
		eCollideShapeType a_ShapeType)
	{
		if (a_ShapeType == eCollideShapeType_Cylinder)
		{
			cMatrixf mtxOffset = cMath::MatrixRotateZ(cMath::ToRad(90));
			return a_pPhysicsWorld->CreateCylinderShape(a_vSize.x, a_vSize.y, &mtxOffset);
		}
		else if (a_ShapeType == eCollideShapeType_Capsule)
		{
			cMatrixf mtxOffset = cMath::MatrixRotateZ(cMath::ToRad(90));
			return a_pPhysicsWorld->CreateCapsuleShape(a_vSize.x, a_vSize.y, &mtxOffset);
		}
		else if (a_ShapeType == eCollideShapeType_Box)
		{
			return a_pPhysicsWorld->CreateBoxShape(a_vSize, NULL);
		}
		else if (a_ShapeType == eCollideShapeType_Sphere)
		{
			return a_pPhysicsWorld->CreateSphereShape(a_vSize.x, NULL);
		}

		return NULL;
	}

	//--------------------------------------------------------------

	cMeshEntity *cMeshLoaderCollada::CreateStaticMeshEntity(cColladaNode *a_pNode, cWorld3D *a_pWorld,
			cColladaGeometry *a_pGeom, bool a_bInRoomGroup,
			tColladaMaterialVec &a_vColladaMaterials,
			tColladaTextureVec &a_vColladaTextures,
			tColladaImageVec &a_vColladaImages)
	{
		tStringVec vParams;
		tString sSepp = "_";
		cString::GetStringVec(a_pNode->m_sName, vParams, &sSepp);

		cMeshEntity *pEntity = NULL;
		iVertexBuffer *pVtxBuffer = NULL;

		tString sMatName = GetMaterialTextureFile(a_pGeom->m_sMaterial, a_vColladaMaterials, a_vColladaTextures,
			a_vColladaImages);

		cMesh *pMesh = NULL;
		cSubMesh *pSubMesh = NULL;

		bool bDrawn = false;

		// Check if mesh is drawn
		if (HasParam(vParams, "nodraw") == false)
		{
			bDrawn = true;

			//Create mesh and sub mesh
			pMesh = efeNew(cMesh, (a_pNode->m_sName, m_pMaterialManager, m_pAnimationManager));
			pSubMesh = pMesh->CreateSubMesh(a_pNode->m_sName + "_Sub");

			//Create vertex buffer
			pVtxBuffer = CreateVertexBuffer(*a_pGeom, eVertexBufferUsageType_Static);

			pVtxBuffer->Transform(a_pNode->m_mtxWorldTransform);

			pSubMesh->SetVertexBuffer(pVtxBuffer);

			//Create triangle data for geometry
			cMath::CreateTriangleData(*pSubMesh->GetTriangleVecPtr(),
				pVtxBuffer->GetIndices(), pVtxBuffer->GetIndexNum(),
				pVtxBuffer->GetArray(eVertexFlag_Position),
				kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)],
				pVtxBuffer->GetVertexNum());

			//Create edges for the geometry
			bool bDoubleSided = false;
			cMath::CreateEdges(*pSubMesh->GetEdgeVecPtr(),
				pVtxBuffer->GetIndices(), pVtxBuffer->GetIndexNum(),
				pVtxBuffer->GetArray(eVertexFlag_Position),
				kvVertexElements[cMath::Log2ToInt(eVertexFlag_Position)],
				pVtxBuffer->GetVertexNum(),
				&bDoubleSided);

			pSubMesh->SetDoubleSided(bDoubleSided);

			pSubMesh->Compile();

			//Add material
			iMaterial *pMaterial;
			if (m_bUseFastMaterial && m_sFastMaterialFile != "")
			{
				pMaterial = m_pMaterialManager->CreateMaterial(m_sFastMaterialFile);
			}
			else
			{
				pMaterial = m_pMaterialManager->CreateMaterial(sMatName);
			}
			if (pMaterial == NULL)
			{
				Error("Couldn't create material '%s' for object '%s'\n", sMatName.c_str(), a_pNode->m_sName.c_str());
				efeDelete(pMesh);
				return NULL;
			}

			pSubMesh->SetMaterial(pMaterial);

			//Create mesh entity
			pEntity = a_pWorld->CreateMeshEntity(a_pNode->m_sName, pMesh, false);
			pEntity->SetMatrix(cMatrixf::Identity);

			pEntity->SetStatic(true);
			pEntity->SetIsSaved(false);

			if (HasParam(vParams, "noshadow"))
				pEntity->SetCastsShadows(false);
			else
				pEntity->SetCastsShadows(true);
		
			if (HasParam(vParams, "nocillide") == false)
			{
				if (bDrawn == false)
				{
					pVtxBuffer = CreateVertexBuffer(*a_pGeom, eVertexBufferUsageType_Static);
					pVtxBuffer->Transform(a_pNode->m_mtxWorldTransform);
				}

				iCollideShape *pShape = a_pWorld->GetPhysicsWorld()->CreateMeshShape(pVtxBuffer);
				iPhysicsBody *pBody = a_pWorld->GetPhysicsWorld()->CreateBody(a_pNode->m_sName, pShape);

				if (pBody)
				{
					pBody->SetMass(0);
					pBody->SetIsSaved(false);
				}
				else
					Error("Body creation failed!\n");

				//pBody->Setbloc
				if (bDrawn)
				{
					if (pEntity->IsShadowCaster() == false ||
						(pSubMesh->GetMaterial() && pSubMesh->GetMaterial()->IsTransparent()))
					{
						
					}
				}
				else
				{
					
				}

				bool bBlocksSound = false;
				if (a_bInRoomGroup) bBlocksSound = true;

				if (HasParam(vParams, "nosoundblock")) bBlocksSound = false;
				if (HasParam(vParams, "soundblock")) bBlocksSound = true;

				pBody->SetBlocksSound(bBlocksSound);

				if (HasParam(vParams, "nocharcollide")) pBody->SetCollideCharacter(false);

				tString sPhysicsMatName = a_pWorld->GetResources()->GetMaterialManager()->GetPhysicsMaterialName(sMatName);
				iPhysicsMaterial *pPhysicsMat = a_pWorld->GetPhysicsWorld()->GetMaterialFromName(sPhysicsMatName);

				if (pPhysicsMat)
					pBody->SetMaterial(pPhysicsMat);

				//Haptic creation
				if (cHaptic::GetIsUsed())
				{
					assert(0);
				}

				if (bDrawn == false)
					efeDelete(pVtxBuffer);
			}
		}

		return pEntity;
	}

	//--------------------------------------------------------------

	cColliderEntity *cMeshLoaderCollada::CreateStaticCollider(cColladaNode *a_pNode, cWorld3D *a_pWorld,
			cColladaGeometry *a_pGeom,
			tColladaMaterialVec &a_vColladaMaterials,
			tColladaTextureVec &a_vColladaTextures,
			tColladaImageVec &a_vColladaImages,
			bool a_bCharacterCollider)
	{
		tStringVec vStrings;
		tString sSepp = "_";
		cString::GetStringVec(a_pNode->m_sName, vStrings, &sSepp);

		tFloatVec vVertexVec;
		tVertexVec &vArray = a_pGeom->m_vVertexVec;
		vVertexVec.resize(vArray.size() * 3);

		for (size_t vtx = 0; vtx < vArray.size(); ++vtx)
		{
			vVertexVec[vtx*3 + 0] = vArray[vtx].pos.x;
			vVertexVec[vtx*3 + 1] = vArray[vtx].pos.y;
			vVertexVec[vtx*3 + 2] = vArray[vtx].pos.z;
		}

		cBoundingVolume TempBV;
		TempBV.AddArrayPoints(&vVertexVec[0], (int)vArray.size());
		TempBV.CreateFromPoints(3);

		tString sShapeType = cString::ToLowerCase(vStrings[1]);
		eCollideShapeType ShapeType = eCollideShapeType_Box;
		cVector3f vShapeSize = TempBV.GetSize() * a_pNode->m_vScale;

		if (sShapeType == "box")
		{
			ShapeType = eCollideShapeType_Box;
		}
		else if (sShapeType == "sphere")
		{
			ShapeType = eCollideShapeType_Sphere;
			vShapeSize *= cVector3f(0.5f);
		}
		else if (sShapeType == "capsule")
		{
			ShapeType = eCollideShapeType_Capsule;
			vShapeSize.x *= 0.5f;
		}
		else if (sShapeType == "cylinder")
		{
			ShapeType = eCollideShapeType_Cylinder;
			vShapeSize.x *= 0.5f;
		}

		iCollideShape *pCollideShape = NULL;

		pCollideShape = CreatePhysicsCollider(a_pWorld->GetPhysicsWorld(), vShapeSize, ShapeType);

		if (pCollideShape == NULL)
		{
			Error("Collider was not created!");
			return NULL;
		}

		iPhysicsBody *pBody = a_pWorld->GetPhysicsWorld()->CreateBody(a_pNode->m_sName, pCollideShape);
		pBody->SetMatrix(a_pNode->m_mtxWorldTransform);

		//pBody->SetBlocksLight(false);

		tString sMatName = GetMaterialTextureFile(a_pGeom->m_sMaterial, a_vColladaMaterials, a_vColladaTextures, a_vColladaImages);

		if (sMatName != "")
		{
			tString sPhysicsMatName = a_pWorld->GetResources()->GetMaterialManager()->GetPhysicsMaterialName(sMatName);

			if (sPhysicsMatName != "")
			{
				iPhysicsMaterial *pPhysicsMat = a_pWorld->GetPhysicsWorld()->GetMaterialFromName(sPhysicsMatName);

				if (pPhysicsMat)
					pBody->SetMaterial(pPhysicsMat);
			}
		}

		bool bBlocksSound = false;
		if (HasParam(vStrings, "soundblock")) bBlocksSound = true;

		pBody->SetBlocksSound(bBlocksSound);
		pBody->SetIsSaved(false);
		pBody->SetCollideCharacter(true);
		if (a_bCharacterCollider)
			pBody->SetCollide(false);
		else
			pBody->SetCollide(true);

		//Haptic creation
		if (cHaptic::GetIsUsed())
		{
			assert(0);
		}

		return a_pWorld->CreateColliderEntity(a_pNode->m_sName, pBody);
	}

	//--------------------------------------------------------------

	void cMeshLoaderCollada::AddSceneObjects(cColladaNode *a_pNode, cWorld3D *a_pWorld,
			tColladaGeometryVec &a_vColladaGeomtries, tColladaLightVec &a_vColladaLights,
			tColladaMaterialVec &a_vColladaMaterials, tColladaTextureVec &a_vColladaTextures,
			tColladaImageVec &a_vColladaImages, cColladaScene *a_pColladaScene)
	{
		//Check if we are dealing with a special type.
		if (a_pNode->m_sName.size() > 1 && a_pNode->m_sName[0] == '_')
		{
			tString sType = cString::Sub(a_pNode->m_sName, 0, 5);
			if (cString::ToLowerCase(sType) == "_room")
			{
				return;
			}

			tStringVec vParams;
			tString sSepp = "_";

			cString::GetStringVec(a_pNode->m_sName, vParams, &sSepp);

			///// COLLIDER /////////////////
			if (cString::ToLowerCase(vParams[0]) == "collider")
			{
				cColladaGeometry *pGeom = GetGeometry(a_pNode->m_sSource, a_vColladaGeomtries);
				if (pGeom)
				{
					cColliderEntity *pCollider = CreateStaticCollider(a_pNode, a_pWorld, pGeom, a_vColladaMaterials,
						a_vColladaTextures, a_vColladaImages, false);

					a_pWorld->GetPortalContainer()->Add(pCollider, true);
				}
				else
				{
					Error("Node '%s' does not have any geometry! Could not create collider!\n", a_pNode->m_sName.c_str());
				}
			}
			///// CHARACTER COLLIDER /////////////////
			else if (cString::ToLowerCase(vParams[0]) == "charcollider")
			{
				cColladaGeometry *pGeom = GetGeometry(a_pNode->m_sSource, a_vColladaGeomtries);
				if (pGeom)
				{
					cColliderEntity *pCollider = CreateStaticCollider(a_pNode, a_pWorld, pGeom, a_vColladaMaterials,
						a_vColladaTextures, a_vColladaImages, true);

					a_pWorld->GetPortalContainer()->Add(pCollider, true);
				}
				else
				{
					Error("Node '%s' does not have any geometry! Could not create charcollider!\n", a_pNode->m_sName.c_str());
				}
			}
			///// SOUND /////////////////
			else if (cString::ToLowerCase(vParams[0]) == "sound" && !(m_Flags & eWorldLoadFlag_NoEntities))
			{
				if (vParams.size() < 3)
					Error("Too few params in sound entity '%s'\n", a_pNode->m_sName.c_str());
				else
				{
					tString sFile = "";
					for (size_t i=1; i<vParams.size()-1; ++i)
					{
						sFile += vParams[i];
						if (i != vParams.size()-2) sFile += "_";
					}
					tString sName = vParams[vParams.size()-1];

					cSoundEntity *pEntity = a_pWorld->CreateSoundEntity(sName, sFile, false);

					if (pEntity == NULL)
					{
						tString sName = vParams[1];
						tString sFile = cString::Sub(a_pNode->m_sName, 7+(int)sName.size()+1);

						pEntity = a_pWorld->CreateSoundEntity(sName, sFile, false);
					}
					if (pEntity) pEntity->SetMatrix(a_pNode->m_mtxWorldTransform);
				}
			}
			///// START /////////////////
			else if (cString::ToLowerCase(vParams[0]) == "start" && !(m_Flags & eWorldLoadFlag_NoEntities))
			{
				if (vParams.size() < 2)
					Error("Too few params in start pos entity '%s'\n", a_pNode->m_sName.c_str());
				else
				{
					tString sName = cString::Sub(a_pNode->m_sName, 7);

					cStartPosEntity *pStart = a_pWorld->CreateStartPos(sName);
					if (pStart)
						pStart->SetMatrix(a_pNode->m_mtxWorldTransform);
				}
			}
			///// REF /////////////////
			else if (cString::ToLowerCase(vParams[0]) == "ref")
			{
				assert(0);
			}
			///// AREA /////////////////
			else if (cString::ToLowerCase(vParams[0]) == "area" && !(m_Flags & eWorldLoadFlag_NoEntities))
			{
				if (vParams.size() < 2)
					Error("Too few params in area entity '%s'\n", a_pNode->m_sName.c_str());
				else
				{
					cColladaGeometry *pGeom = GetGeometry(a_pNode->m_sSource, a_vColladaGeomtries);

					tFloatVec vVertexVec;
					tVertexVec &vArray = pGeom->m_vVertexVec;
					vVertexVec.resize(vArray.size() * 3);

					for (size_t vtx=0; vtx<vArray.size(); ++vtx)
					{
						vVertexVec[vtx*3 + 0] = vArray[vtx].pos.x;
						vVertexVec[vtx*3 + 1] = vArray[vtx].pos.y;
						vVertexVec[vtx*3 + 2] = vArray[vtx].pos.z;
					}

					cBoundingVolume TempBV;
					TempBV.AddArrayPoints(&vVertexVec[0], (int)vArray.size());
					TempBV.CreateFromPoints(3);

					tString sType;
					tString sName;
					if (vParams.size() < 3 || (m_Flags & eWorldLoadFlag_NoGameEntites))
					{
						sType = "";
						if (vParams.size() < 3)
							sName = cString::ToLowerCase(vParams[1]);
						else
							sName = cString::Sub(a_pNode->m_sName, (int)vParams[0].size() + (int)vParams[1].size() + 3);
					}
					else
					{
						sType = cString::ToLowerCase(vParams[1]);
						sName = cString::Sub(a_pNode->m_sName, (int)vParams[0].size() + (int)vParams[1].size() + 3);
					}

					cVector3f vSize = TempBV.GetSize() * a_pNode->m_vScale;

					if (sType != "")
					{
						iArea3DLoader *pLoader = a_pWorld->GetResources()->GetArea3DLoader(sType);
						if(pLoader)
						{
							pLoader->Load(sName, vSize, a_pNode->m_mtxWorldTransform, a_pWorld);
						}
					}

					//Create engine area
					cAreaEntity *pArea = a_pWorld->CreateAreaEntity(sName);
					pArea->m_mtxTransform = a_pNode->m_mtxWorldTransform;
					pArea->m_sType = sType;
					pArea->m_vSize = vSize;
				}
			}
		}
		//Load light or geometry
		else
		{
			//The node has a source
			if (a_pNode->m_sSource != "")
			{
				//Get number of chars in prefix
				int lPrefixChars = 1;
				while (lPrefixChars < a_pNode->m_sName.size() &&
					a_pNode->m_sName[lPrefixChars] != '_' &&
					a_pNode->m_sName[lPrefixChars] != '\0')
					lPrefixChars++;

				//Get prefix
				tString sPrefix = cString::ToLowerCase(cString::Sub(a_pNode->m_sName, 0, lPrefixChars));

				// Load source from external file
				if (a_pNode->m_bSourceIsFile)
				{
					tString sFile = cString::SetFileExt(cString::GetFileName(a_pNode->m_sSource), "");

					if (m_Flags && eWorldLoadFlag_NoGameEntites)
					{
						//Do nothing
					}
					else if (sPrefix == "static")
					{
						cMesh *pMesh = m_pMeshManager->CreateMesh(sFile);
						if (pMesh)
						{
							//Create the entity
							cMeshEntity *pEntity = a_pWorld->CreateMeshEntity(a_pNode->m_sName, pMesh, false);

							pEntity->SetMatrix(a_pNode->m_mtxWorldTransform);

							a_pWorld->GetPortalContainer()->Add(pEntity, true);

							//Set parameters
							tStringVec vParams;
							tString sSepp = "_";
							cString::GetStringVec(a_pNode->m_sName, vParams, &sSepp);

							if (HasParam(vParams, "noshadow"))
								pEntity->SetCastsShadows(false);
							else
								pEntity->SetCastsShadows(true);
						}
					}
					else
					{
						tString sEntityFile = cString::SetFileExt(sFile, "ent");

						a_pWorld->CreateEntity(a_pNode->m_sName, a_pNode->m_mtxWorldTransform, sEntityFile, true);
					}
				}
				// Load source from collada data
				else
				{
					//Create an entity from the geometry
					cColladaGeometry *pGeom = GetGeometry(a_pNode->m_sSource, a_vColladaGeomtries);
					if (pGeom)
					{
						cMeshEntity *pEntity = CreateStaticMeshEntity(a_pNode, a_pWorld,
							pGeom, false,
							a_vColladaMaterials, a_vColladaTextures, a_vColladaImages);

						if (pEntity)
						{
							a_pWorld->GetPortalContainer()->Add(pEntity, true);
						}
					}
					else if (!(m_Flags & eWorldLoadFlag_NoLights))
					{
						cColladaLight *pColladaLight = GetLight(a_pNode->m_sSource, a_vColladaLights);
						if (pColladaLight)
						{
							tStringVec vParams;
							tString sSepp = "_";
							cString::GetStringVec(a_pNode->m_sName, vParams, &sSepp);

							//check if this is a dynamic light
							bool bStatic = true;
							int lParamAdd = 0;
							if (cString::ToLowerCase(vParams[0]) == "dynamic")
							{
								bStatic = false;
								lParamAdd = 1;
							}

							tString sLightName = "";
							tString sLightFile = "";

							if ((int)vParams.size() >= 2 + lParamAdd)
							{
								for (size_t i=lParamAdd; i<vParams.size()-1; ++i)
								{
									sLightFile += vParams[i];
									if (i != vParams.size()-2) sLightFile += "_";
								}
								sLightFile = cString::SetFileExt(sLightFile, "lnt");

								sLightName = vParams[vParams.size()-1];
							}
							//No file parameter
							else
							{
								sLightName = vParams[lParamAdd];
							}

							//Load the specific light type
							if (pColladaLight->m_sType == "point")
							{
								cLight3DPoint *pLight = a_pWorld->CreateLightPoint(sLightName, false);

								pLight->SetMatrix(a_pNode->m_mtxWorldTransform);
								pColladaLight->m_DiffuseColor.a = a_pNode->m_vScale.y;
								pLight->SetDiffuseColor(pColladaLight->m_DiffuseColor);
								pLight->SetFarAttenuation(a_pNode->m_vScale.x);

								if (a_pNode->m_vScale.z < 0.1f)
									pLight->SetCastShadows(false);
								else
									pLight->SetCastShadows(true);

								if (bStatic) pLight->SetStatic(bStatic);

								if (m_bRestricStaticLightToSector) pLight->SetOnlyAffectInSector(true);

								a_pWorld->GetPortalContainer()->Add(pLight, bStatic);

								if (sLightFile != "")
									pLight->LoadXMLProperties(sLightFile);
							}
							else if (pColladaLight->m_sType == "spot")
							{
								assert(0);
							}
							else
							{
								Warning("Invalid light type '%s' for light '%s'\n", pColladaLight->m_sType.c_str(),
									a_pNode->m_sName.c_str());
							}
						}
						else
						{
							Warning("Source '%s' is not found!\n", a_pNode->m_sSource.c_str());
						}
					}
				}
			}
		}

		tColladaNodeListIt ChildIt = a_pNode->m_lstChildren.begin();
		for (; ChildIt != a_pNode->m_lstChildren.end(); ChildIt++)
		{
			AddSceneObjects(*ChildIt, a_pWorld, a_vColladaGeomtries, a_vColladaLights,
				a_vColladaMaterials, a_vColladaTextures, a_vColladaImages, a_pColladaScene);
		}
	}
}