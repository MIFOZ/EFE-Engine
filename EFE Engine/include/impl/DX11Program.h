#ifndef EFE_HLSLPROGRAM_H
#define EFE_HLSLPROGRAM_H

#include <D3D11.h>
#include <d3dcompiler.h>

#include "impl/Platform.h"
#include "impl/LowLevelGraphicsDX11.h"

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GpuProgram.h"
#include "graphics/GpuProgramParameter.h"
#include "graphics/GpuProgramParameterManager.h"

#include <array>

namespace efe
{

	class cConstant
	{
	public:
		tString name;
		ubyte *vsData;
		ubyte *gsData;
		ubyte *psData;
		int vsBuffer;
		int gsBuffer;
		int psBuffer;
	};

	typedef std::map<tString, cConstant*> tConstantMap;
	typedef tConstantMap::iterator tConstantMapIt;

	struct cSampler
	{
		char *name;
		int vsIndex;
		int gsIndex;
		int psIndex;
	};

	typedef std::map<tString, cSampler> tSamplerMap;
	typedef tSamplerMap::iterator tSamplerMapIt;

// 	struct cDX11InputLayout : public iInputSignature
// 	{
// 		cDX11InputLayout()
// 		{
// 			m_pInputLayout = NULL;
// 		}
// 
// 		~cDX11InputLayout()
// 		{
// 			if (m_pInputLayout)m_pInputLayout->Release();
// 		}
// 		ID3D10Blob *m_pInputLayout;
// 	};

	struct cConstantBufferLayout
	{
		D3D11_SHADER_BUFFER_DESC Description;
		std::vector<D3D11_SHADER_VARIABLE_DESC> Variables;
		std::vector<D3D11_SHADER_TYPE_DESC> Types;
		tGpuProgramParameterVec Parameters;
		iGpuProgramParameter *pParameterRef;
	};

	typedef std::vector<cConstantBufferLayout> tConstantBufferLayoutVec;
	typedef tConstantBufferLayoutVec::iterator tConstantBufferLayoutVecIt;

	struct cGpuProgramInputBindDesc
	{
		cGpuProgramInputBindDesc(D3D11_SHADER_INPUT_BIND_DESC a_Desc)
		{
			m_sName = tString(a_Desc.Name);
			m_Type = a_Desc.Type;
			m_lBindPoint = a_Desc.BindPoint;
			m_lBindCount = a_Desc.BindCount;
			m_Flags = a_Desc.uFlags;
			m_ReturnType = a_Desc.ReturnType;
			m_Dimension = a_Desc.Dimension;
			m_lSamplesNum = a_Desc.NumSamples;
			m_pParameter = NULL;
		}

		tString m_sName;
		D3D_SHADER_INPUT_TYPE m_Type;
		unsigned int m_lBindPoint;
		unsigned int m_lBindCount;
		tFlag m_Flags;
		D3D11_RESOURCE_RETURN_TYPE m_ReturnType;
		D3D_SRV_DIMENSION m_Dimension;
		unsigned int m_lSamplesNum;
		iGpuProgramParameter *m_pParameter;
	};

	class cDX11PipelineManager;

	class cDX11Program : public iGpuProgram
	{
	public:
		cDX11Program(tString a_sName, iLowLevelGraphics *a_pLowLevelGfx, ID3D11Device *a_pDevice, ID3D11DeviceContext *a_pContext, cGpuProgramParameterManager *a_pParameterManager, eGpuProgramType a_Type);
		~cDX11Program();

		bool Reload();
		void Unload();
		void Destroy();

		tString GetProgramName(){return m_sName;}

		bool CreateFromFile(const tString &a_sFile, const tString &a_sEntry, tGpuProgramDefineVec &a_vDefines);

		void Bind();
		void UnBind();

		bool SetRaw(const tString &a_sName, const void *a_pData, const int a_lSize);

		ID3DBlob *GetCompiledProgram(){return m_pCompiledProgram;}

		void BindParameters(cDX11PipelineManager *a_pPipeline, cGpuProgramParameterManager *a_pParamManager);

		std::vector<D3D11_SIGNATURE_PARAMETER_DESC> &GetInputSignatureVec(){return m_vInputSignatureParameters;}

	protected:
		bool GenerateReflection();
		void InitConstantBuffers();

		iLowLevelGraphics *m_pLowLevelGfx;

		tString m_sFile;
		tString m_sEntry;

		tConstantBufferLayoutVec m_vConstantBuffers;

		ID3D11Device *m_pDevice;
		ID3D11DeviceContext *m_pContext;

		ID3DBlob *m_pCompiledProgram;

		ID3D11VertexShader *m_pVertexShader;
		ID3D11PixelShader *m_pPixelShader;
		ID3D11GeometryShader *m_pGeometryShader;

		cGpuProgramParameterManager *m_pParameterManager;

		std::vector<D3D11_SIGNATURE_PARAMETER_DESC> m_vInputSignatureParameters;
		std::vector<D3D11_SIGNATURE_PARAMETER_DESC> m_vOutputSignatureParameters;

		std::vector<cGpuProgramInputBindDesc> m_vResourceBindings;

	private:
		// Only for render effect
		tGpuProgramParameterVec GetConstantBufferParameterVec();
	};
};

#endif