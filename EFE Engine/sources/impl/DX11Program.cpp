#pragma comment (lib, "d3dcompiler.lib")

#include "impl/DX11Program.h"
#include "impl/DX11Texture.h"
#include "impl/DX11ProgramBuffer.h"
#include "impl/RenderPipelineSystem/DX11PipelineManager.h"

#include "graphics/GpuProgramParameterManager.h"

#include "system/String.h"

#include <D3D11Shader.h>
#include <D3Dcompiler.h>

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	int SamplerComp(const void *a_pA, const void *a_pB)
	{
		return strcmp(((cSampler *)a_pA)->name, ((cSampler *)a_pB)->name);
	}

	cDX11Program::cDX11Program(tString a_sName, iLowLevelGraphics *a_pLowLevelGfx, ID3D11Device *a_pDevice, ID3D11DeviceContext *a_pContext, cGpuProgramParameterManager *a_pParameterManager, eGpuProgramType a_Type)
		: iGpuProgram(a_sName, a_Type)
	{
		m_pLowLevelGfx = a_pLowLevelGfx;

		m_pDevice = a_pDevice;
		m_pContext = a_pContext;

		m_pParameterManager = a_pParameterManager;

		m_pVertexShader = NULL;
		m_pPixelShader = NULL;
		m_pGeometryShader = NULL;

	}

	cDX11Program::~cDX11Program()
	{
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	bool cDX11Program::Reload()
	{
		return false;
	}

	//--------------------------------------------------------------

	void cDX11Program::Unload()
	{
		
	}

	//--------------------------------------------------------------

	void cDX11Program::Destroy()
	{
		
	}

	//--------------------------------------------------------------

	bool cDX11Program::CreateFromFile(const tString &a_sFile, const tString &a_sEntry, tGpuProgramDefineVec &a_vDefines)
	{
		FILE *pFile = fopen(a_sFile.c_str(), "rb");

		fseek(pFile, 0, SEEK_END);
		int lLength = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		char *sShaderText = efeNewArray(char, lLength + 1);
		fread(sShaderText, lLength, 1, pFile);
		fclose(pFile);
		sShaderText[lLength] = '\0';
		
		UINT lCompileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		lCompileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

		D3D_SHADER_MACRO *pDefinesVec = new D3D_SHADER_MACRO[a_vDefines.size() + 1];
		for (int i = 0; i < a_vDefines.size(); i++)
		{
			 pDefinesVec[i].Name = a_vDefines[i].m_sName.c_str();
			 pDefinesVec[i].Definition = a_vDefines[i].m_sDefinition.c_str();
		}
		pDefinesVec[a_vDefines.size()].Name = NULL;
		pDefinesVec[a_vDefines.size()].Definition = NULL;

		tString sModel = "";
		switch (m_ProgramType)
		{
		case efe::eGpuProgramType_Vertex: sModel = "vs_5_0"; break;
		case efe::eGpuProgramType_Geometry: sModel = "gs_5_0"; break;
		case efe::eGpuProgramType_Pixel: sModel = "ps_5_0"; break;
		case efe::eGpuProgramType_LastEnum:
		default: break;
		}

		ID3DBlob *pShaderBuf = NULL;
		ID3DBlob *pErrorBuf = NULL;

		HRESULT hr;
		if (FAILED(hr = D3DCompile(sShaderText, lLength, a_sFile.c_str(), pDefinesVec, NULL, a_sEntry.c_str(), sModel.c_str(), lCompileFlags,
			NULL, &pShaderBuf, &pErrorBuf)))
		{
			fclose(pFile);
			efeDeleteArray(sShaderText);
			FatalError("Could not compile gpu program '%s': %s\n", m_sName.c_str(), (const char *)pErrorBuf->GetBufferPointer());
			return false;
		}

		m_pCompiledProgram = pShaderBuf;

		switch (m_ProgramType)
		{
		case efe::eGpuProgramType_Vertex:
			{
				hr = m_pDevice->CreateVertexShader(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), 0, &m_pVertexShader);
				break;
			}			
		case efe::eGpuProgramType_Geometry:
			{
				hr = m_pDevice->CreateGeometryShader(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), 0, &m_pGeometryShader);
				break;
			}	
		case efe::eGpuProgramType_Pixel:
			{
				hr = m_pDevice->CreatePixelShader(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), 0, &m_pPixelShader);
				break;
			}	
		default:
			hr = E_FAIL;
			break;
		}

		if (FAILED(hr))
		{
			fclose(pFile);
			efeDeleteArray(sShaderText);
			pShaderBuf->Release();
			Error("Could not create gpu program '%s'\n", m_sName.c_str());
			return false;
		}

		GenerateReflection();

		InitConstantBuffers();

		fclose(pFile);
		efeDeleteArray(sShaderText);

		return true;
	}

	void cDX11Program::Bind()
	{
		switch (m_ProgramType)
		{
		case efe::eGpuProgramType_Vertex:
			m_pContext->VSSetShader(m_pVertexShader, NULL, 0);
			break;
		case efe::eGpuProgramType_Geometry:
			m_pContext->GSSetShader(m_pGeometryShader, NULL, 0);
			break;
		case efe::eGpuProgramType_Pixel:
			m_pContext->PSSetShader(m_pPixelShader, NULL, 0);
			break;
		default: break;
		}
	}

	void cDX11Program::UnBind()
	{
	
	}

// 	void cDX11Program::UpdateConstants()
// 	{
// 		if (m_lVSCBuffersNum || m_lGSCBuffersNum || m_lPSCBuffersNum)
// 		{
// 			m_pContext->VSSetConstantBuffers(0, m_lVSCBuffersNum, m_pVSConstants);
// 			m_pContext->GSSetConstantBuffers(0, m_lGSCBuffersNum, m_pGSConstants);
// 			m_pContext->PSSetConstantBuffers(0, m_lPSCBuffersNum, m_pPSConstants);
// 
// 			for (int i = 0; i < m_lVSCBuffersNum; i++)
// 			{
// 				if (m_pVSDirty[i])
// 				{
// 					m_pContext->UpdateSubresource(m_pVSConstants[i], 0, NULL, m_vVSConstMems[i].data(), 0, 0);
// 					m_pVSDirty[i] = false;
// 				}
// 			}
// 
// 			for (int i = 0; i < m_lGSCBuffersNum; i++)
// 			{
// 				if (m_pGSDirty[i])
// 				{
// 					m_pContext->UpdateSubresource(m_pGSConstants[i], 0, NULL, m_vGSConstMems[i].data(), 0, 0);
// 					m_pGSDirty[i] = false;
// 				}
// 			}
// 
// 			for (int i = 0; i < m_lPSCBuffersNum; i++)
// 			{
// 				if (m_pPSDirty[i])
// 				{
// 					m_pContext->UpdateSubresource(m_pPSConstants[i], 0, NULL, m_vPSConstMems[i].data(), 0, 0);
// 					m_pPSDirty[i] = false;
// 				}
// 			}
// 		}
// 	}

	bool cDX11Program::SetRaw(const tString &a_sName, const void *a_pData, const int a_lSize)
	{
// 		int lMinConstant = 0;
// 		int lMaxConstant = m_lConstantsNum - 1;
// 
// 		tConstantMapIt it = m_mapConstants.find(a_sName);
// 		if (it == m_mapConstants.end())
// 		{
// 			if (m_bDebugInfo)
// 				Log("Invalid constant '%s' in '%s'\n", a_sName.c_str(), m_sName.c_str());
// 			return false;
// 		}
// 
// 		cConstant *pConst = (*it).second;
// 
// 		if (pConst->vsData)
// 		{
// 			if (memcmp(pConst->vsData, a_pData, a_lSize))
// 			{
// 				memcpy(pConst->vsData, a_pData, a_lSize);
// 				m_pVSDirty[pConst->vsBuffer] = true;
// 			}
// 		}
// 
// 		if (pConst->gsData)
// 		{
// 			if (memcmp(pConst->gsData, a_pData, a_lSize))
// 			{
// 				memcpy(pConst->gsData, a_pData, a_lSize);
// 				m_pGSDirty[pConst->gsBuffer] = true;
// 			}
// 		}
// 
// 		if (pConst->psData)
// 		{
// 			if (memcmp(pConst->psData, a_pData, a_lSize))
// 			{
// 				memcpy(pConst->psData, a_pData, a_lSize);
// 				m_pPSDirty[pConst->psBuffer] = true;
// 			}
// 		}

		return true;
	}

	//--------------------------------------------------------------

	void cDX11Program::BindParameters(cDX11PipelineManager *a_pPipeline, cGpuProgramParameterManager *a_pParamManager)
	{
		for (cGpuProgramInputBindDesc &Binding : m_vResourceBindings)
		{
			UINT lSlot = Binding.m_lBindPoint;

			switch (Binding.m_Type)
			{
			case D3D_SIT_CBUFFER: 
			case D3D_SIT_TBUFFER:
				a_pPipeline->BindConstatntBufferParameter(m_ProgramType, Binding.m_pParameter, lSlot, a_pParamManager);
				break;
			case D3D_SIT_SAMPLER:
				a_pPipeline->BindSamplerStateParameter(m_ProgramType, Binding.m_pParameter, lSlot, a_pParamManager);
				break;
			case D3D_SIT_TEXTURE:
			case D3D_SIT_STRUCTURED:
			case D3D_SIT_BYTEADDRESS:
				a_pPipeline->BindShaderResourceParameter(m_ProgramType, Binding.m_pParameter, lSlot, a_pParamManager);
				break;
			case D3D_SIT_UAV_RWTYPED:
			case D3D_SIT_UAV_RWSTRUCTURED:
			case D3D_SIT_UAV_RWBYTEADDRESS:
			case D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				Log("Trying to set UAV resource type for program '%s'\n", m_sName);
				break;
			default:
				break;
			}
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	/*CGparameter cDXProgram::GetParam(const tString &a_sName, CGtype a_Type)
	{
		CGparameter Param = cgGetNamedParameter(m_Program, a_sName.c_str());
		if (Param==NULL) return NULL;

		CGtype type = cgGetParameterType(Param);
		if (type!=a_Type)return NULL;

		return Param;
	}*/

	/*bool cDX11Program::CreateFromStrings(const char *a_pVSText, const char *a_pGSText, const char *a_pPSText, const int a_lVSLine, const int a_lGSLine, const int a_lPSLine,
					const char *a_pHeader, const char *a_pExtra, const char *a_pFileName/*, const char **a_pAttributesNames, const int a_lAttribsNum)
	{
		if (a_pVSText == NULL && a_pGSText == NULL && a_pPSText == NULL) return false;

		ID3DBlob *pShaderBuf = NULL;
		ID3DBlob *pErrorBuf = NULL;

		ID3D11ShaderReflection *pVSRefl = NULL;
		ID3D11ShaderReflection *pGSRefl = NULL;
		ID3D11ShaderReflection *pPSRefl = NULL;

		UINT lCompileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		lCompileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

		if (a_pVSText != NULL)
		{
			tString sShaderString;
			if (a_pExtra != NULL) sShaderString.append(a_pExtra);
			if (a_pHeader != NULL) sShaderString.append(a_pHeader);
			sShaderString.append("#line ");
			sShaderString.append(cString::ToString(a_lVSLine + 1));
			sShaderString.append("\r\n");
			sShaderString.append(a_pVSText);

			if (SUCCEEDED(D3DCompile(sShaderString.c_str(), sShaderString.size(), a_pFileName, NULL, NULL, "main", "vs_5_0", lCompileFlags,
							NULL, &pShaderBuf, &pErrorBuf)))
			{
				if (SUCCEEDED(m_pLowGfx->GetDevice()->CreateVertexShader(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(),
								NULL, &m_pVertexShader)))
				{
					cDXInputSignature *pIS = static_cast<cDXInputSignature*>(m_pInputSignature);
					D3DGetInputSignatureBlob(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &pIS->m_pInputSignature);
					D3DReflect(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void **)&pVSRefl);
				}
			}
			else
			{
				FatalError("Could not create  vertex program '%s': %s\n", m_sName.c_str(), (const char *)pErrorBuf->GetBufferPointer());
			}

			SAFE_RELEASE(pShaderBuf);
			SAFE_RELEASE(pErrorBuf);

			if (m_pVertexShader == NULL) return false;
		}

		if (a_pGSText != NULL)
		{
			tString sShaderString;
			if (a_pExtra != NULL) sShaderString.append(a_pExtra);
			if (a_pHeader != NULL) sShaderString.append(a_pHeader);
			sShaderString.append("#line ");
			sShaderString.append(cString::ToString(a_lGSLine + 1));
			sShaderString.append("\r\n");
			sShaderString.append(a_pGSText);

			//Log("%s", sShaderString.c_str());

			if (SUCCEEDED(D3DCompile(sShaderString.c_str(), sShaderString.size(), a_pFileName, NULL, NULL, "main", "gs_5_0", lCompileFlags,
							NULL, &pShaderBuf, &pErrorBuf)))
			{
				if (SUCCEEDED(m_pLowGfx->GetDevice()->CreateGeometryShader(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(),
								NULL, &m_pGeometryShader)))
					D3DReflect(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void **)&pGSRefl);
			}
			else
			{
				FatalError("Could not create  geometry program '%s': %s\n", m_sName.c_str(), (const char *)pErrorBuf->GetBufferPointer());
			}

			SAFE_RELEASE(pShaderBuf);
			SAFE_RELEASE(pErrorBuf);

			if (m_pGeometryShader == NULL) return false;
		}

		if (a_pPSText != NULL)
		{
			tString sShaderString;
			if (a_pExtra != NULL) sShaderString.append(a_pExtra);
			if (a_pHeader != NULL) sShaderString.append(a_pHeader);
			sShaderString.append("#line ");
			sShaderString.append(cString::ToString(a_lPSLine + 1));
			sShaderString.append("\r\n");
			sShaderString.append(a_pPSText);

			if (SUCCEEDED(D3DCompile(sShaderString.c_str(), sShaderString.size(), a_pFileName, NULL, NULL, "main", "ps_5_0", lCompileFlags,
							NULL, &pShaderBuf, &pErrorBuf)))
			{
				if (SUCCEEDED(m_pLowGfx->GetDevice()->CreatePixelShader(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(),
								NULL, &m_pPixelShader)))
					D3DReflect(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), IID_ID3D11ShaderReflection, (void **)&pPSRefl);
			}
			else
			{
				FatalError("Could not create  pixel program '%s': %s\n", m_sName.c_str(), (const char *)pErrorBuf->GetBufferPointer());
			}

			SAFE_RELEASE(pShaderBuf);
			SAFE_RELEASE(pErrorBuf);

			if (m_pPixelShader == NULL) return false;
		}

		D3D11_SHADER_DESC descVS, descGS, descPS;
		if (pVSRefl)
		{
			pVSRefl->GetDesc(&descVS);

			if (descVS.ConstantBuffers)
			{
				m_lVSCBuffersNum = descVS.ConstantBuffers;
				m_pVSConstants = new ID3D11Buffer * [m_lVSCBuffersNum];
				m_vVSConstMems.resize(m_lVSCBuffersNum);
				m_pVSDirty = new bool [m_lVSCBuffersNum];
			}
		}
		if (pGSRefl)
		{
			pGSRefl->GetDesc(&descGS);

			if (descGS.ConstantBuffers)
			{
				m_lGSCBuffersNum = descGS.ConstantBuffers;
				m_pGSConstants = new ID3D11Buffer * [m_lGSCBuffersNum];
				m_vGSConstMems.resize(m_lGSCBuffersNum);
				m_pGSDirty = new bool [m_lGSCBuffersNum];
			}
		}
		if (pPSRefl)
		{
			pPSRefl->GetDesc(&descPS);

			if (descPS.ConstantBuffers)
			{
				m_lPSCBuffersNum = descPS.ConstantBuffers;
				m_pPSConstants = new ID3D11Buffer * [m_lPSCBuffersNum];
				m_vPSConstMems.resize(m_lPSCBuffersNum);
				m_pPSDirty = new bool [m_lPSCBuffersNum];
			}
		}

		D3D11_SHADER_BUFFER_DESC descSB;

		D3D11_BUFFER_DESC descCB;
		descCB.Usage = D3D11_USAGE_DEFAULT;
		descCB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		descCB.CPUAccessFlags = 0;
		descCB.MiscFlags = 0;
		descCB.StructureByteStride = 0;

		for (int i = 0; i < m_lVSCBuffersNum; i++)
		{
			pVSRefl->GetConstantBufferByIndex(i)->GetDesc(&descSB);

			descCB.ByteWidth = descSB.Size;
			m_pLowGfx->GetDevice()->CreateBuffer(&descCB, NULL, &m_pVSConstants[i]);

			m_vVSConstMems[i].resize(descSB.Size);
			for (UINT k = 0; k < descSB.Variables; k++)
			{
				D3D11_SHADER_VARIABLE_DESC descSV;
				pVSRefl->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&descSV);

				cConstant *pConst = new cConstant();
				pConst->name = descSV.Name;
				pConst->vsData = m_vVSConstMems[i].data() + descSV.StartOffset;
				pConst->gsData = NULL;
				pConst->psData = NULL;
				pConst->vsBuffer = i;
				pConst->gsBuffer = -1;
				pConst->psBuffer = -1;
				m_mapConstants.insert(tConstantMap::value_type(pConst->name, pConst));
			}

			m_pVSDirty[i] = false;
		}

		for (int i = 0; i < m_lGSCBuffersNum; i++)
		{
			pGSRefl->GetConstantBufferByIndex(i)->GetDesc(&descSB);

			descCB.ByteWidth = descSB.Size;
			m_pLowGfx->GetDevice()->CreateBuffer(&descCB, NULL, &m_pGSConstants[i]);

			m_vGSConstMems[i].resize(descSB.Size);
			for (UINT k = 0; k < descSB.Variables; k++)
			{
				D3D11_SHADER_VARIABLE_DESC descSV;
				pGSRefl->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&descSV);

				tConstantMapIt it = m_mapConstants.find(descSV.Name);
				if (it == m_mapConstants.end())
				{
					cConstant *pConst = new cConstant();
					pConst->name = descSV.Name;
					pConst->vsData = NULL;
					pConst->gsData = m_vGSConstMems[i].data() + descSV.StartOffset;
					pConst->psData = NULL;
					pConst->vsBuffer = -1;
					pConst->gsBuffer = i;
					pConst->psBuffer = -1;
					m_mapConstants.insert(tConstantMap::value_type(pConst->name, pConst));
				}
				else
				{
					cConstant *pConst = (*it).second;
					pConst->gsData = m_vGSConstMems[i].data() + descSV.StartOffset;
					pConst->gsBuffer = i;
				}
			}

			m_pGSDirty[i] = false;
		}

		for (int i = 0;i < m_lPSCBuffersNum; i++)
		{
			pPSRefl->GetConstantBufferByIndex(i)->GetDesc(&descSB);

			descCB.ByteWidth = descSB.Size;
			m_pLowGfx->GetDevice()->CreateBuffer(&descCB, NULL, &m_pPSConstants[i]);

			m_vPSConstMems[i].resize(descSB.Size);
			for (UINT k = 0;k < descSB.Variables;k++)
			{
				D3D11_SHADER_VARIABLE_DESC descSV;
				pPSRefl->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&descSV);

				tConstantMapIt it = m_mapConstants.find(descSV.Name);
				if (it == m_mapConstants.end())
				{
					cConstant *pConst = new cConstant();
					pConst->name = descSV.Name;
					pConst->vsData = NULL;
					pConst->gsData = NULL;
					pConst->psData = m_vPSConstMems[i].data() + descSV.StartOffset;
					pConst->vsBuffer = -1;
					pConst->gsBuffer = -1;
					pConst->psBuffer = i;
					m_mapConstants.insert(tConstantMap::value_type(pConst->name, pConst));
				}
				else
				{
					cConstant *pConst = (*it).second;
					pConst->psData = m_vPSConstMems[i].data() + descSV.StartOffset;
					pConst->psBuffer = i;
				}
			}

			m_pPSDirty[i] = false;
		}

		UINT lMaxVSRes = pVSRefl ? descVS.BoundResources : 0;
		UINT lMaxGSRes = pGSRefl ? descGS.BoundResources : 0;
		UINT lMaxPSRes = pPSRefl ? descPS.BoundResources : 0;

		int lMaxResources = lMaxVSRes + lMaxGSRes + lMaxPSRes;
		if (lMaxResources)
		{
			m_lTexturesNum = 0;
			m_lSamplersNum = 0;

			D3D11_SHADER_INPUT_BIND_DESC descSI;
			for (UINT i = 0; i < lMaxVSRes;i++)
			{
				pVSRefl->GetResourceBindingDesc(i, &descSI);

				if (descSI.Type == D3D_SIT_TEXTURE)
				{
					
					cSampler Texture;
					Texture.vsIndex = descSI.BindPoint;
					Texture.gsIndex = -1;
					Texture.psIndex = -1;
					m_mapTextures.insert(tSamplerMap::value_type(descSI.Name, Texture));
					m_lTexturesNum++;
				}
				else if (descSI.Type == D3D_SIT_SAMPLER)
				{
					cSampler Sampler;
					Sampler.vsIndex = descSI.BindPoint;
					Sampler.gsIndex = -1;
					Sampler.psIndex = -1;
					m_mapSamplers.insert(tSamplerMap::value_type(descSI.Name, Sampler));
					m_lSamplersNum++;
				}
			}

			UINT lMaxTexture = m_lTexturesNum;
			UINT lMaxSampler = m_lSamplersNum;
			for (UINT i = 0; i < lMaxGSRes;i++)
			{
				pGSRefl->GetResourceBindingDesc(i, &descSI);

				if (descSI.Type == D3D_SIT_TEXTURE)
				{
					tSamplerMapIt it = m_mapTextures.find(descSI.Name);
					if (it == m_mapTextures.end())
					{
						cSampler Texture;
						Texture.vsIndex = -1;
						Texture.gsIndex = descSI.BindPoint;
						Texture.psIndex = -1;
						m_mapTextures.insert(tSamplerMap::value_type(descSI.Name, Texture));
					}
					else
					{
						cSampler Texture = (*it).second;
						Texture.gsIndex = descSI.BindPoint;
					}
				}
				else if (descSI.Type == D3D_SIT_SAMPLER)
				{
					tSamplerMapIt it = m_mapSamplers.find(descSI.Name);
					if (it == m_mapSamplers.end())
					{
						cSampler Sampler;
						Sampler.vsIndex = -1;
						Sampler.gsIndex = descSI.BindPoint;
						Sampler.psIndex = -1;
						m_mapSamplers.insert(tSamplerMap::value_type(descSI.Name, Sampler));
					}
					else
					{
						cSampler Sampler = (*it).second;
						Sampler.gsIndex = descSI.BindPoint;
					}
				}
			}

			lMaxTexture = m_lTexturesNum;
			lMaxSampler = m_lSamplersNum;

			for (UINT i = 0; i < lMaxPSRes;i++)
			{
				pPSRefl->GetResourceBindingDesc(i, &descSI);

				if (descSI.Type == D3D_SIT_TEXTURE)
				{
					tSamplerMapIt it = m_mapTextures.find(descSI.Name);
					if (it == m_mapTextures.end())
					{
						cSampler Texture;
						Texture.vsIndex = -1;
						Texture.gsIndex = -1;
						Texture.psIndex = descSI.BindPoint;
						m_mapTextures.insert(tSamplerMap::value_type(descSI.Name, Texture));
					}
					else
					{
						cSampler Texture = (*it).second;
						Texture.psIndex = descSI.BindPoint;
					}
				}
				else if (descSI.Type == D3D_SIT_SAMPLER)
				{
					tSamplerMapIt it = m_mapSamplers.find(descSI.Name);
					if (it == m_mapSamplers.end())
					{
						cSampler Sampler;
						Sampler.vsIndex = -1;
						Sampler.gsIndex = -1;
						Sampler.psIndex = descSI.BindPoint;
						m_mapSamplers.insert(tSamplerMap::value_type(descSI.Name, Sampler));
					}
					else
					{
						cSampler Sampler = (*it).second;
						Sampler.psIndex = descSI.BindPoint;
					}
				}
			}
		}

		if (pVSRefl) pVSRefl->Release();
		if (pGSRefl) pGSRefl->Release();
		if (pPSRefl) pPSRefl->Release();

		tConstantMapIt it = m_mapConstants.begin();
		/*for (; it != m_mapConstants.end(); ++it)
		{
			Log("%s, ", (*it).second->name.c_str());
		}
		
		Log("\n\n");
		return true;
	}*/

	bool cDX11Program::GenerateReflection()
	{
		ID3D11ShaderReflection *pReflection;
		HRESULT hr = D3DReflect(m_pCompiledProgram->GetBufferPointer(), m_pCompiledProgram->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection);

		if (FAILED(hr))
			return false;

		D3D11_SHADER_DESC desc;
		pReflection->GetDesc(&desc);

		for (unsigned int i = 0; i < desc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC inputDesc;
			pReflection->GetInputParameterDesc(i, &inputDesc);
			m_vInputSignatureParameters.push_back(inputDesc);
		}
		for (unsigned int i = 0; i < desc.OutputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC outputDesc;
			pReflection->GetOutputParameterDesc(i, &outputDesc);
			m_vOutputSignatureParameters.push_back(outputDesc);
		}

		for (unsigned int i = 0; i < desc.ConstantBuffers; i++)
		{
			ID3D11ShaderReflectionConstantBuffer *pConstBuffer = pReflection->GetConstantBufferByIndex(i);

			D3D11_SHADER_BUFFER_DESC bufferDesc;
			pConstBuffer->GetDesc(&bufferDesc);

			if (bufferDesc.Type == D3D_CT_CBUFFER || bufferDesc.Type == D3D_CT_TBUFFER)
			{
				cConstantBufferLayout BufferLayout;
				BufferLayout.Description = bufferDesc;
				BufferLayout.pParameterRef = m_pParameterManager->GetConstantBufferParameter(tString(bufferDesc.Name));

				for (size_t var = 0; var < BufferLayout.Description.Variables; var++)
				{
					ID3D11ShaderReflectionVariable *pVariable = pConstBuffer->GetVariableByIndex(var);
					D3D11_SHADER_VARIABLE_DESC var_desc;
					pVariable->GetDesc(&var_desc);
					BufferLayout.Variables.push_back(var_desc);

					ID3D11ShaderReflectionType *pType = pVariable->GetType();
					D3D11_SHADER_TYPE_DESC type_desc;
					pType->GetDesc(&type_desc);
					BufferLayout.Types.push_back(type_desc);

					iGpuProgramParameter *pParam = NULL;
					if (type_desc.Class == D3D_SVC_VECTOR)
						pParam = m_pParameterManager->GetVectorParameter(tString(var_desc.Name));
					else if (type_desc.Class == D3D_SVC_MATRIX_COLUMNS ||
						type_desc.Class == D3D_SVC_MATRIX_ROWS)
					{
						unsigned int lCount = type_desc.Elements;
						if (lCount == 0)
							pParam = m_pParameterManager->GetMatrixParameter(tString(var_desc.Name));
						else
							pParam = m_pParameterManager->GetMatrixArrayParameter(tString(var_desc.Name), lCount);
					}

					BufferLayout.Parameters.push_back(pParam);
				}

				m_vConstantBuffers.push_back(BufferLayout);
			}
		}

		for (unsigned int i = 0; i < desc.BoundResources; i++)
		{
			D3D11_SHADER_INPUT_BIND_DESC resource_desc;
			pReflection->GetResourceBindingDesc(i, &resource_desc);
			cGpuProgramInputBindDesc BindDesc(resource_desc);

			if (resource_desc.Type == D3D_SIT_CBUFFER || resource_desc.Type == D3D_SIT_TBUFFER)
				BindDesc.m_pParameter = m_pParameterManager->GetConstantBufferParameter(tString(resource_desc.Name));
			else if (resource_desc.Type == D3D_SIT_TEXTURE || resource_desc.Type == D3D_SIT_STRUCTURED)
				BindDesc.m_pParameter = m_pParameterManager->GetResourceParameter(tString(resource_desc.Name));
			else if (resource_desc.Type == D3D_SIT_SAMPLER)
				BindDesc.m_pParameter = m_pParameterManager->GetSamplerStateParameter(tString(resource_desc.Name));
			else if (resource_desc.Type == D3D_SIT_UAV_RWTYPED)
			{

			}

			m_vResourceBindings.push_back(BindDesc);
		}
	}

	void cDX11Program::InitConstantBuffers()
	{
		for (size_t i=0; i<m_vConstantBuffers.size(); i++)
		{
			cConstantBufferLayout &Buffer = m_vConstantBuffers[i];
			if (Buffer.Description.Type == D3D11_CT_CBUFFER)
			{
				cDX11ProgramConstantBuffer *pBuffer = static_cast<cDX11ProgramConstantBuffer*>(m_pParameterManager->GetConstantBufferParameterBuffer(Buffer.pParameterRef));

				if (pBuffer == NULL)
				{
					cDX11ProgramBufferConfig Config;
					Config.SetConstantBuffer(Buffer.Description.Size, true);

					pBuffer = static_cast<cDX11ProgramConstantBuffer*>(m_pLowLevelGfx->CreateGpuProgramBuffer(eGpuProgramBufferType_Constant, &Config, NULL));

					Buffer.pParameterRef->SetParameterData(pBuffer);

					for (size_t j = 0; j < Buffer.Variables.size(); j++)
					{
						cConstantBufferMapping Mapping;

						Mapping.m_pParameter = Buffer.Parameters[j];
						Mapping.m_lOffset = Buffer.Variables[j].StartOffset;
						Mapping.m_lSize = Buffer.Variables[j].Size;
						Mapping.m_lElementsNum = Buffer.Types[j].Elements;
						Mapping.m_VarClass = Buffer.Types[j].Class;

						pBuffer->AddMapping(Mapping);
					}
				}
				else
				{
					if (pBuffer->GetAutoUpdate())
					{
						for (size_t j = 0; j < Buffer.Variables.size(); j++)
						{
							cConstantBufferMapping Mapping;

							Mapping.m_pParameter = Buffer.Parameters[j];
							Mapping.m_lOffset = Buffer.Variables[j].StartOffset;
							Mapping.m_lSize = Buffer.Variables[j].Size;
							Mapping.m_lElementsNum = Buffer.Types[j].Elements;
							Mapping.m_VarClass = Buffer.Types[j].Class;

							if (pBuffer->ContainsMapping(j, Mapping) == false)
							{
								Error("Constant buffer '%s' is already mapped with mapping '%s'!\n", Buffer.pParameterRef->GetName().c_str(), Mapping.m_pParameter->GetName().c_str());
							}
						}
					}
				}
			}
		}
	}

	//--------------------------------------------------------------

	tGpuProgramParameterVec cDX11Program::GetConstantBufferParameterVec()
	{
		tGpuProgramParameterVec vParameters;
		for (cConstantBufferLayout &Layout : m_vConstantBuffers)
		{
			vParameters.push_back(Layout.pParameterRef);
		}
		return vParameters;
	}
}