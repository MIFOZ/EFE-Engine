#ifndef EFE_GPUPROGRAM_BUFFER_H
#define EFE_GPUPROGRAM_BUFFER_H

namespace efe
{
	enum eGpuProgramBufferType
	{
		eGpuProgramBufferType_Constant,
		eGpuProgramBufferType_VertexIndex,
		eGpuProgramBufferType_LastEnum
	};

	enum eBufferUsage
	{
		eBufferUsage_Static,
		eBufferUsage_Dynamic,
		eBufferUsage_Stream
	};

	class iGpuProgramBuffer
	{
	public:
		iGpuProgramBuffer(eGpuProgramBufferType a_Type)
			: m_Type(a_Type){}
		virtual ~iGpuProgramBuffer(){}

	private:
		eGpuProgramBufferType m_Type;
	};

	class iGpuProgramBufferConfig
	{
	public:
		iGpuProgramBufferConfig()
		{
			SetDefaults();
		}
		virtual ~iGpuProgramBufferConfig(){}

		virtual void SetDefaults(){}

		virtual void SetConstantBuffer(unsigned int a_lSize, bool a_bDynamic)=0;
		virtual void SetVertexBuffer(eBufferUsage a_Usage, unsigned int a_lSize, unsigned int a_lStride)=0;
		virtual void SetIndexBuffer(eBufferUsage a_Usage, unsigned int a_lSize, unsigned int a_lStride)=0;


		/*virtual void SetDefaults()=0;
		virtual void SetDefaults()=0;
		virtual void SetDefaults()=0;
		virtual void SetDefaults()=0;*/
	};
};

#endif