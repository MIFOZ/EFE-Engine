#include "graphics/Renderer3D.h"

#include "math/Math.h"

#include "system/LowLevelSystem.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/Resources.h"
#include "resources/LowLevelResources.h"
#include "resources/TextureManager.h"
#include "graphics/VertexBuffer.h"
#include "graphics/MeshCreator.h"
#include "scene/Camera3D.h"
#include "scene/Entity3D.h"
#include "graphics/RenderList.h"
#include "graphics/Renderable.h"
#include "scene/World3D.h"
#include "scene/RenderableContainer.h"
#include "scene/Light3D.h"
#include "math/BoundingVolume.h"
#include "resources/GpuProgramManager.h"
#include "graphics/GpuProgram.h"

#include <d3dx10.h>

namespace efe
{
	//////////////////////////////////////////////////////////////
	// RENDER SETTINGS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cRenderSettings::cRenderSettings()
	{
		m_bFogActive = false;
		m_fFogStart = 5.0f;
		m_fFogEnd = 5.0f;
		m_FogColor = cColor(1,1);
		m_bFogCulling = false;
	}

	void cRenderSettings::Clear()
	{
		m_lLastShadowAlgo = 0;

		m_bDepthTest = true;

		m_AlphaMode = eMaterialAlphaMode_Solid;
		m_BlendMode = eMaterialBlendMode_None;

		m_pVertexProgram = NULL;
		m_bVertexUsusLight = false;
		m_pPixelProgram = NULL;

		m_pSector = NULL;

		m_bUsesLight = false;
		m_bUsesEye = false;

		m_bMatrixWasNULL = false;

		for (int i = 0; i < MAX_TEXTUREUNITS; i++)
		{
			m_pTexture[i] = NULL;
		}

		m_pVtxBuffer = NULL;
	}

	cRenderer3D::cRenderer3D(iLowLevelGraphics *a_pLowLevelGraphics, cResources *a_pResources,
					cMeshCreator* a_pMeshCreator, cRenderList * a_pRenderList)
	{
		Log("  Creating Renderer3D\n");

		m_pLowLevelGraphics = a_pLowLevelGraphics;
		m_pLowLevelResources = a_pResources->GetLowLevel();
		m_pResources = a_pResources;
		m_pMeshCreator = a_pMeshCreator;

		//m_pCameraPositionWriter = m_ParameterContainer.GetVectorParameter()

		//Render Targets
		cVector2f vSize = m_pLowLevelGraphics->GetScreenSize();
		cVector2l vLSize((int)vSize.x, (int)vSize.y);
		int lSamples = m_pLowLevelGraphics->GetMultisampling();
		m_pBaseRT = m_pLowLevelGraphics->CreateRenderTarget(vLSize, 1, 1, 1, eFormat_RGB10A2, lSamples, false, eTextureTarget_2D);
		m_pGeometryRT = m_pLowLevelGraphics->CreateRenderTarget(vLSize, 1, 1, 1, eFormat_RGBA16F, lSamples, false, eTextureTarget_2D);
		m_pNormalRT = m_pLowLevelGraphics->CreateRenderTarget(vLSize, 1, 1, 1, eFormat_RGBA8S, lSamples, false, eTextureTarget_2D);
		m_pDepthRT = m_pLowLevelGraphics->CreateRenderDepth(vLSize, 1, eFormat_D24S8, true, lSamples);
		m_pStencilMaskRT = m_pLowLevelGraphics->CreateRenderDepth(vLSize, 1, eFormat_D24S8, false, 1);
		//m_pResultRT = m_pLowLevelGraphics->CreateRenderTarget(vLSize, 1, 1, 1, eFormat_RGBA8, 1, true, eTextureTarget_2D);
		//m_pGeometryRT = m_pLowLevelGraphics->CreateRenderTarget(vLSize, 1, 1, 1, eFormat_RG16F, 1, false, eTextureTarget_2D);

		//Skybox
		m_pSkyBoxTexture = NULL;
		m_bAutoDestroySkybox = false;
		m_bSkyBoxActive = false;
		//m_SkyBoxColor = cColor(1,1);

		m_RenderSettings.m_AmbientColor = cColor(0,1);

		m_pRenderList = a_pRenderList;

		m_DebugFlags = 0;

		m_bLog = true;

		m_fRenderTime = 0;

		m_bRefractionUsed = true;

		m_vVtxRect.resize(4);
		m_vVtxRect[0] = cVertex(cVector3f(0,0,0), cVector3f(0,1,0), cColor(1,1));
		m_vVtxRect[1] = cVertex(cVector3f(1,0,0), cVector3f(1,1,0), cColor(1,1));
		m_vVtxRect[2] = cVertex(cVector3f(1,1,0), cVector3f(1,0,0), cColor(1,1));
		m_vVtxRect[3] = cVertex(cVector3f(0,1,40), cVector3f(0,0,0), cColor(1,1));

		m_RenderSettings.m_pLowLevel = m_pLowLevelGraphics;
		m_RenderSettings.m_bLog = false;
		m_RenderSettings.m_ShowShadows = eRendererShowShadows_All;
		m_RenderSettings.m_pTempIndexArray = efeNewArray(unsigned int, 60000);

		Log("   Load Renderer3D gpu programs:\n");

		cGpuProgramManager *pProgramManager = a_pResources->GetGpuProgramManager();

		//m_pFillBuffersVtxProgram = pProgramManager->CreateProgram("FillBuffers.hlsl", "vsMain", eGpuProgramType_Vertex);
		//if (m_pFillBuffersVtxProgram == NULL)
		//	FatalError("Couldn't load '%s' program!\n", m_pFillBuffersVtxProgram->GetName().c_str());
		//m_pFillBuffersPxlProgram = pProgramManager->CreateProgram("FillBuffers.hlsl", "psMain", eGpuProgramType_Pixel);

		//m_pAmbientVtxProgram = pProgramManager->CreateProgram("Ambient.hlsl", "vsMain", eGpuProgramType_Vertex);
		//m_pAmbientPxlProgram = pProgramManager->CreateProgram("Ambient.hlsl", "psMain", eGpuProgramType_Pixel);

		//m_pLightingVtxProgram = pProgramManager->CreateProgram("Lighting_vs.hlsl", "main", eGpuProgramType_Vertex);
		//m_pLightingPxlProgram = pProgramManager->CreateProgram("Lighting_ps.hlsl", "main", eGpuProgramType_Pixel);

		Log("   Create sampler states\n");
		/*m_pBaseFilter = m_pLowLevelGraphics->CreateSamplerState(eTextureFilter_TrilinearAniso,	eTextureAddressMode_Wrap,
																								eTextureAddressMode_Wrap,
																								eTextureAddressMode_Wrap);*/

		m_pTriAnisoFilter = m_pLowLevelGraphics->CreateSamplerState(eTextureFilter_TrilinearAniso,eTextureAddressMode_Wrap,
																								eTextureAddressMode_Wrap,
																								eTextureAddressMode_Wrap);

		m_pPointClamp = m_pLowLevelGraphics->CreateSamplerState(eTextureFilter_Nearest,	eTextureAddressMode_Clamp,
																						eTextureAddressMode_Clamp,
																						eTextureAddressMode_Clamp);

		/*Log("   Extrude\n");

		//m_RenderSettings.m_pVtxExtrudeProgram = pProgramManager->CreateProgram("ShadowExtrude_vp.cg","main",eGpuProgramType_Vertex);
		if (m_RenderSettings.m_pVtxExtrudeProgram == NULL)
			Error("Couldn't load 'ShadowExtrude_vp.cg' program! Shadows will be disabled.\n");

		//m_RenderSettings.m_pPxlExtrudeProgram = pProgramManager->CreateProgram("ShadowExtrude_pp.cg","main",eGpuProgramType_Pixel);

		Log("   Diffuse Vertex\n");
		//m_pDiffuseVtxProgram = pProgramManager->CreateProgram("Diffuse_Color_vp.cg","main",
																//eGpuProgramType_Vertex);
		//if(m_pDiffuseVtxProgram==NULL)
			Error("Couldn't load 'Diffuse_Color_vp.cg'\n");

		//Log("   Diffuse Pixel\n");
		//m_pDiffusePxlProgram = pProgramManager->CreateProgram("Diffuse_Color_pp.cg","main",eGpuProgramType_Pixel);

		//Log("   Fog\n");
		//m_pSolidFogVtxProgram = pProgramManager->CreateProgram("Fog_Solid_vp.cg","main",eGpuProgramType_Vertex);
		//if(m_pSolidFogVtxProgram==NULL)
			//Error("Couldn't load 'Fog_Solid_vp.cg'\n");

		//m_pSolidFogPxlProgram = pProgramManager->CreateProgram("Fog_Solid_pp.cg","main",eGpuProgramType_Pixel);

		Log("   Creating fog textures: ");
		unsigned char *pFogArray = efeNewArray(unsigned char,256*2);

		Log("Solid ");
		pTex = m_pLowLevelGraphics->CreateTexture("FogLinearSolid", false, eTextureType_Normal, eTextureTarget_1D);

		for (int i=0;i<256;++i)
		{
			pFogArray[i*2] = 255;
			pFogArray[i*2 + 1] = 255 - (unsigned char)i;
		}

		//pTex->CreateFromBitmap

		m_pFogLinearSolidTexture = pTex;

		Log("Additive ");
		pTex = m_pLowLevelGraphics->CreateTexture("FogLinearAdd",false,eTextureType_Normal,eTextureTarget_1D);

		Log("\n");*/

		Log("   init sky box\n");
		
		m_pSkyBoxVtxProgram = pProgramManager->CreateProgram("SkyBox.hlsl", "vsMain", eGpuProgramType_Vertex);
		m_pSkyBoxPxlProgram = pProgramManager->CreateProgram("SkyBox.hlsl", "psMain", eGpuProgramType_Pixel);
		if (m_pSkyBoxPxlProgram == NULL)
			FatalError("Couldn't load 'SkyBox.hlsl' program!\n");

		InitSkyBox();

		// Rasterizer states
		Log("   create raster states\n");
		m_pCullNone = m_pLowLevelGraphics->CreateRasterizerState(eCullMode_None, eFillMode_Solid);
		m_pCullBack = m_pLowLevelGraphics->CreateRasterizerState(eCullMode_Back);
		m_pCullFront = m_pLowLevelGraphics->CreateRasterizerState(eCullMode_Front);

		// Depth stencil states
		m_pDepthTest = m_pLowLevelGraphics->CreateDepthStencilState(true, true, eDepthTestFunc_LessOrEqual);
		m_pDepthNoTest = m_pLowLevelGraphics->CreateDepthStencilState(false, false);
		m_pStencilSet = m_pLowLevelGraphics->CreateDepthStencilState(false, false, eDepthTestFunc_GreaterOrEqual,
			true, 0xFF, eStencilFunc_Always, eStencilOp_Replace, eStencilOp_Replace, eStencilOp_Replace);
		m_pStencilTest = m_pLowLevelGraphics->CreateDepthStencilState(false, false, eDepthTestFunc_GreaterOrEqual,
			true, 0xFF, eStencilFunc_Equal, eStencilOp_Keep, eStencilOp_Keep, eStencilOp_Keep);

		// Blend state
		//m_pBlendAdd = m_pLowLevelGraphics->CreateBlendState(eBlendFunc_One, eBlendFunc_One);

		/*m_pDiffuseProgram = pProgramManager->CreateProgram("DiffuseProgram.shd", "main", eGpuProgramType_Pixel);

		cVertexFormatDesc format[] = 
		{
			0, eAttributeType_Normal, eAttributeFormat_Float, 3,
			0, eAttributeType_Vertex, eAttributeFormat_Float, 4,
			0, eAttributeType_Color, eAttributeFormat_Float, 4,
			0, eAttributeType_TexCoord, eAttributeFormat_Float, 3,
			//0, eAttributeType_TexCoord, eAttributeFormat_Float, 4
		}; 
		m_pPlainVF = m_pLowLevelGraphics->CreateVertexFormat(format, 4, m_pDiffuseProgram);*/

		m_pLightSphere = m_pMeshCreator->CreateSphereVertexBuffer(3);

		Log("  Renderer3D created\n");
	}

	cRenderer3D::~cRenderer3D()
	{
		efeDeleteArray(m_RenderSettings.m_pTempIndexArray);

		if (m_pSkyBox) efeDelete(m_pSkyBox);
	}

	//--------------------------------------------------------------

	void cRenderer3D::UpdateRenderList(cWorld3D *a_pWorld, cCamera3D *a_pCamera, float a_fFrameTime)
	{
		m_pRenderList->Clear();

		m_pRenderList->SetFrameTime(a_fFrameTime);
		m_pRenderList->SetCamera(a_pCamera);

		//Set the frustum
		m_RenderSettings.m_pFrustum = a_pCamera->GetFrustum();

		if (m_RenderSettings.m_bFogActive && m_RenderSettings.m_bFogCulling)
		{
			float fCornerDist = (m_RenderSettings.m_fFogEnd * 2.0f) /
								cos(a_pCamera->GetFOV() * a_pCamera->GetAspect()*0.5f);

			m_FogBV.SetSize(fCornerDist);
			m_FogBV.SetPosition(a_pCamera->GetPosition());
		}

		a_pWorld->GetRenderContainer()->GetVisible(m_RenderSettings.m_pFrustum, m_pRenderList);

		m_pRenderList->Compile();
	}

	void cRenderer3D::RenderWorld(cWorld3D *a_pWorld, cCamera3D *a_pCamera, float a_fFrameTime)
	{
		m_fRenderTime += a_fFrameTime;

		if (m_DebugFlags & eRendererDebugFlag_LogRendering)
		{
			m_bLog = true;
			m_RenderSettings.m_bLog = true;
		}
		else if(m_bLog)
		{
			m_bLog = false;
			m_RenderSettings.m_bLog = false;
		}
		m_RenderSettings.m_DebugFlags = m_DebugFlags;

		BeginRendering(a_pCamera);

		m_RenderSettings.Clear();

		if (m_bLog) Log("Rendering ZBuffer:\n");
		RenderZ(a_pCamera);

		//m_pLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc_Equal);

		if (m_bLog) Log("Rendering Diffuse:\n");
		RenderDiffuse(a_pCamera);
		//RenderOpaque(a_pCamera);

		//RenderAmbient();

		//RenderLight(a_pCamera);

		//RenderSkyBox(a_pCamera);
	}

	//-----------------------------------------------------------

	void cRenderer3D::SetSkyBox(iTexture *a_pTexture, bool a_bAutoDestroy)
	{
		if (m_pSkyBoxTexture && m_bAutoDestroySkybox)
			m_pResources->GetTextureManager()->Destroy(m_pSkyBoxTexture);

		m_bAutoDestroySkybox = a_bAutoDestroy;
		m_pSkyBoxTexture = a_pTexture;
	}

	void cRenderer3D::SetSkyBoxActive(bool a_bX)
	{
		m_bSkyBoxActive = a_bX;
	}

	//-----------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//-----------------------------------------------------------

	void cRenderer3D::BeginRendering(cCamera3D *a_pCamera)
	{
		//m_pLowLevelGraphics->SetClearColorActive(true);
		//m_pLowLevelGraphics->SetClearDepthActive(true);

		m_pLowLevelGraphics->SetProgramVec3f("CamPos", a_pCamera->GetEyePosition());

		m_pLowLevelGraphics->SetClearColor(cColor(0, 1, 0, 1));
		m_pLowLevelGraphics->SetClearColorActive(true);
		m_pLowLevelGraphics->SetClearDepth(1);
		m_pLowLevelGraphics->SetClearDepthActive(true);
		m_pLowLevelGraphics->SetClearStencilActive(false);

		m_pLowLevelGraphics->ClearScreen();

		iLowLevelGraphics::s_mtxView = a_pCamera->GetViewMatrix();
		iLowLevelGraphics::s_mtxProjection = a_pCamera->GetProjectionMatrix();
		
		m_mtxViewProj = cMath::MatrixMul(a_pCamera->GetProjectionMatrix(), a_pCamera->GetViewMatrix());

		cVector2f vSize = m_pLowLevelGraphics->GetScreenSize();
		m_vScaleBias = cColor(0.5f * vSize.x, -0.5f * vSize.y, 0.5f * vSize.x, 0.5f * vSize.y);

		iTexture *bufferRTs[] = {m_pBaseRT, m_pNormalRT, m_pGeometryRT};
		//m_pLowLevelGraphics->ChangeRenderTargets(bufferRTs, 3, m_pDepthRT);
		//m_pLowLevelGraphics->ClearRenderTarget(m_pBaseRT, cColor(0.0f, 1.0f, 0.0f, 1.0f));
		//m_pLowLevelGraphics->ClearRenderTarget(m_pGeometryRT, cColor(1000.0f, 1000.0f, 1000.0f, 1.0f));
		//m_pLowLevelGraphics->ClearDepthTarget(m_pDepthRT, 1);
		//m_pLowLevelGraphics->SetClearStencil(0);
		//m_pLowLevelGraphics->ClearDepthTarget(m_pDepthRT, 0.0f);

		m_RenderSettings.m_pCamera = a_pCamera;
	}

	//-----------------------------------------------------------

	void cRenderer3D::InitSkyBox()
	{
		m_pSkyBox = m_pMeshCreator->CreateSkyBoxVertexBuffer(1);
	}

	//-----------------------------------------------------------

	void cRenderer3D::RenderSkyBox(cCamera3D *a_pCamera)
	{
		if (m_bSkyBoxActive == false) return;

		if (m_bLog) Log(" Drawing skybox\n");

		static float r = 0.0f;

		//r += 0.02f;

		cMatrixf mtxWorld = cMath::MatrixRotateY(r);
		mtxWorld.SetTranslation(cVector3f(-40, -20, 0));

		m_pLowLevelGraphics->Reset();
		m_pLowLevelGraphics->ChangeToMainFramebuffer(false, m_pDepthRT);
		m_pLowLevelGraphics->SetRasterizerState(m_pCullNone);
		m_pLowLevelGraphics->SetDepthState(m_pDepthTest);
		m_pLowLevelGraphics->BindProgram(m_pSkyBoxVtxProgram, eGpuProgramType_Vertex);
		//cMatrixf mtxView = a_pCamera->GetViewMatrix();
		cMatrixf mtxSky = cMatrixf::Identity;
		float fFarClip = a_pCamera->GetFarClipPlane();
		float fSide = sqrt((fFarClip * fFarClip) / 3) * 0.95;
		mtxSky.m[0][0] = fSide;
		mtxSky.m[1][1] = fSide;
		mtxSky.m[2][2] = fSide;
		mtxSky = cMath::MatrixMul(mtxSky, a_pCamera->GetViewMatrix());
		mtxSky.SetTranslation(0);
		cMatrixf mtxViewProj = cMath::MatrixMul(a_pCamera->GetProjectionMatrix(), mtxSky);
		m_pLowLevelGraphics->SetShaderConstantMatrixf("ViewProj", mtxViewProj);
		m_pLowLevelGraphics->SetShaderConstantMatrixf("Rotation", cMath::MatrixRotateZ(0.0f));
		m_pLowLevelGraphics->SetTexture("Sky", m_pSkyBoxTexture);
		m_pLowLevelGraphics->SetProgramVec3f("CamPos", a_pCamera->GetEyePosition());
		m_pLowLevelGraphics->SetSamplerState("Filter", m_pTriAnisoFilter);
		m_pLowLevelGraphics->FlushRendering();

		//m_pLowLevelGraphics->GetContext()->IASetInputLayout(m_pSkyBoxVertexFormat);
		//m_pLowLevelGraphics->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		unsigned int lOffset = 0;
		unsigned int lStride = sizeof(cColor);
// 		m_pLowLevelGraphics->GetContext()->IASetVertexBuffers(0, 1, pBuffers, &lStride, &lOffset);
// 		m_pLowLevelGraphics->GetContext()->IASetIndexBuffer(m_pSkyBox->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
// 		
// 		m_pLowLevelGraphics->GetContext()->DrawIndexed(36, 0, 0);
	}

	//-----------------------------------------------------------

	void cRenderer3D::RenderZ(cCamera3D *a_pCamera)
	{
		cRenderNode *pNode = m_pRenderList->GetRootNode(eRenderListDrawType_Normal,
			eMaterialRenderType_LinearDepthNormal,0);

		m_RenderSettings.m_pLight = NULL;
		pNode->Render(&m_RenderSettings);
	}

	//-----------------------------------------------------------

	void cRenderer3D::RenderDiffuse(cCamera3D *a_pCamera)
	{
		cRenderNode *pNode = m_pRenderList->GetRootNode(eRenderListDrawType_Normal,
			eMaterialRenderType_Perspective,0);

		m_RenderSettings.m_pLight = NULL;
		pNode->Render(&m_RenderSettings);
	}

	//-----------------------------------------------------------

	void cRenderer3D::RenderOpaque(cCamera3D *a_pCamera)
	{
		m_pLowLevelGraphics->Reset();
		m_pLowLevelGraphics->SetRasterizerState(m_pCullBack);
		m_pLowLevelGraphics->BindProgram(m_pFillBuffersVtxProgram, eGpuProgramType_Vertex);
		//m_pLowLevelGraphics->SetShaderConstantVec3f("CamPos", a_pCamera->GetEyePosition());
		m_pLowLevelGraphics->SetDepthState(m_pDepthTest);
		//m_pLowLevelGraphics->SetShaderConstantColor4f("ScaleBias", m_vScaleBias);
		m_pLowLevelGraphics->SetSamplerState("Filter", m_pPointClamp);
		m_pLowLevelGraphics->SetShaderConstantMatrixf("ViewProj", m_mtxViewProj);
		m_pLowLevelGraphics->SetShaderConstantMatrixf("View", a_pCamera->GetViewMatrix());
		//m_pLowLevelGraphics->SetShaderConstantFloat("FarClipPlane", a_pCamera->GetFarClipPlane());
		//m_pLowLevelGraphics->SetVertexFormat(m_pBaseVF);
		m_pLowLevelGraphics->FlushRendering();

		cRenderableIterator objectIt = m_pRenderList->GetObjectIt();
		while (objectIt.HasNext())
		{
			iRenderable *pObject = objectIt.Next();

			if (pObject->GetRenderType() == eRenderableType_Normal)
			{
				iTexture *pDiff = pObject->GetMaterial()->GetTexture(eMaterialTexture_Diffuse);
				iTexture *pNorm = pObject->GetMaterial()->GetTexture(eMaterialTexture_NMap);

				m_pLowLevelGraphics->SetTexture("Base", pDiff);
				m_pLowLevelGraphics->SetTexture("Bump", pNorm);
				m_pLowLevelGraphics->SetShaderConstantMatrixf("WVP", cMath::MatrixMul(m_mtxViewProj, pObject->GetWorldMatrix()));
				m_pLowLevelGraphics->SetShaderConstantMatrixf("World", pObject->GetWorldMatrix());
				m_pLowLevelGraphics->FlushConstants();
				m_pLowLevelGraphics->FlushTextures();

				iVertexBuffer *pVB = pObject->GetVertexBuffer();
				pVB->Bind();
				pVB->Draw();
			}
		}
	}

	//-----------------------------------------------------------

	void cRenderer3D::RenderAmbient()
	{
		m_pLowLevelGraphics->ChangeToMainFramebuffer(true);

		m_pLowLevelGraphics->Reset();
		m_pLowLevelGraphics->SetRasterizerState(m_pCullNone);
		m_pLowLevelGraphics->SetDepthState(m_pDepthNoTest);
		m_pLowLevelGraphics->BindProgram(m_pAmbientVtxProgram, eGpuProgramType_Vertex);
		m_pLowLevelGraphics->SetShaderConstantColor4f("Ambient", m_RenderSettings.m_AmbientColor);
		m_pLowLevelGraphics->SetTexture("Base", m_pBaseRT);
		cVector3f vCol = cVector3f(m_RenderSettings.m_AmbientColor.r,m_RenderSettings.m_AmbientColor.g,m_RenderSettings.m_AmbientColor.b);
		
		if (m_pLowLevelGraphics->GetMultisampling() == 1)
			m_pLowLevelGraphics->SetSamplerState("Filter", m_pPointClamp);
		m_pLowLevelGraphics->FlushRendering();

		// Draw a full-screen triangle
		//m_pLowLevelGraphics->GetContext()->Draw(3, 0);
	}

	void cRenderer3D::RenderLight(cCamera3D *a_pCamera)
	{
		if (m_DebugFlags & eRendererDebugFlag_DisableLighting)
			return;

		m_pLowLevelGraphics->Reset();
		m_pLowLevelGraphics->SetDepthState(m_pDepthNoTest);
		m_pLowLevelGraphics->BindProgram(m_pLightingVtxProgram, eGpuProgramType_Vertex);
		m_pLowLevelGraphics->SetRasterizerState(m_pCullFront);
		m_pLowLevelGraphics->SetBlendState(m_pBlendAdd);
		m_pLowLevelGraphics->SetShaderConstantMatrixf("View", a_pCamera->GetViewMatrix());
		m_pLowLevelGraphics->SetShaderConstantMatrixf("ViewProj", m_mtxViewProj);
		float fFar = a_pCamera->GetFarClipPlane();
		float fNear = a_pCamera->GetNearClipPlane();
		float fProjectionA = fFar / (fFar - fNear);
		float fProjectionB = (-fFar * fNear) / (fFar - fNear);
		m_pLowLevelGraphics->SetShaderConstantFloat("Far", fFar);
		m_pLowLevelGraphics->SetShaderConstantFloat("Near", fNear);
		m_pLowLevelGraphics->SetShaderConstantMatrixf("InvProj", cMath::MatrixInverse(a_pCamera->GetProjectionMatrix()));
		cMatrixf mtxInvViewProj = cMath::MatrixInverse(m_mtxViewProj);
		cVector2f vSreenSize = m_pLowLevelGraphics->GetScreenSize();
		m_pLowLevelGraphics->SetTexture("Base", m_pBaseRT);
		m_pLowLevelGraphics->SetTexture("Normal", m_pNormalRT);
		m_pLowLevelGraphics->SetTexture("Depth", m_pDepthRT);
		m_pLowLevelGraphics->SetTexture("Geometry", m_pGeometryRT);
		//m_pLowLevelGraphics->SetVertexFormat(m_pFormatPos);
		m_pLowLevelGraphics->FlushRendering();

		cLight3DIterator lightIt = m_pRenderList->GetLightIt();
		while (lightIt.HasNext())
		{
			iLight *pLight = lightIt.Next();

			float fRadius = pLight->GetNearAttenuation();
			cVector3f vWorldPos = pLight->GetLightPosition();

			float fInvRadius = 1.0f/fRadius;

			cMatrixf mtxView = a_pCamera->GetViewMatrix();

			/*cVector3f vScreenPos*/m_vLightPos = cMath::MatrixMul(mtxView, vWorldPos);
			m_vLightPos.z = -m_vLightPos.z;
			float fZ1 = m_vLightPos.z + fRadius;

			cMatrixf mtxProj = a_pCamera->GetProjectionMatrix();
			cVector2f vZW = cVector2f(-mtxProj.m[2][2], mtxProj.m[2][3]);

			/*if (fZ1 > a_pCamera->GetNearClipPlane())
			{*/
				float fZ0 = cMath::Max(m_vLightPos.z - fRadius, a_pCamera->GetNearClipPlane());

				cVector2f vZBounds;
				vZBounds.y = cMath::Saturate(vZW.x + vZW.y / fZ0);
				vZBounds.x = cMath::Saturate(vZW.x + vZW.y / fZ1);

				m_pLowLevelGraphics->SetProgramVec3f("LightPos", vWorldPos);

				m_pLowLevelGraphics->SetProgramVec3f("LightViewPos", m_vLightPos);
				m_pLowLevelGraphics->SetShaderConstantFloat("Radius", fRadius);
				//m_pLowLevelGraphics->SetShaderConstantFloat("InvRadius", fInvRadius);
				m_pLowLevelGraphics->SetShaderConstantVec2f("ZBounds", vZBounds);
				//cMatrixf mtxLight = cMath::MatrixTranslate(cVector3f(pLight->GetLightPosition()));
				//m_pLowLevelGraphics->SetShaderConstantMatrixf("ViewProj", /*cMath::MatrixMul(*/m_mtxViewProj/*, mtxLight)*/);
				m_pLowLevelGraphics->FlushConstants();

				m_pLightSphere->Bind();
				m_pLightSphere->Draw();
			//}

		}
	}
}