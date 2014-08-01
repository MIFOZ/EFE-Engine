#ifndef EFE_SCRIPT_H
#define EFE_SCRIPT_H

#include "resources/ResourceBase.h"
#include <angelscript.h>

#define SCRIPT_DEFINE_FUNC(return, funcname) \
	SCRIPT_FUNC_BEGIN(funcname, return, "") \
	SCRIPT_RETURN_CALL_##return funcname; \
	SCRIPT_FUNC_END(funcname, return)

#define SCRIPT_DEFINE_FUNC_1(return, funcname, arg0) \
	SCRIPT_FUNC_BEGIN(funcname, return, #arg0) \
	SCRIPT_ARG_##arg0(0); \
	SCRIPT_RETURN_CALL_##return funcname(_arg0); \
	SCRIPT_FUNC_END(funcname, return)

#define SCRIPT_FUNC_BEGIN(funcname, return, args) \
	namespace GenericScript { \
	static std::string funcname##_return = #return; \
	static std::string funcname##_arg = args; \
	void funcname##_Generic(asIScriptGeneric *gen) { \
	SCRIPT_RETURN_##return;

#define SCRIPT_FUNC_END(funcname, return) \
	SCRIPT_SET_RETURN_##return; \
} \
}
		// Parameter Macros
#define SCRIPT_ARG_string(n) std::string _arg##n = *(std::string *)gen->GetArgObject(n)

	// Return Value Macros
#define SCRIPT_RETURN_string std::string _ret;
#define SCRIPT_RETURN_CALL_string  _ret =

#define SCRIPT_RETURN_void
#define SCRIPT_RETURN_CALL_void
#define SCRIPT_SET_RETURN_void

#if defined (AS_MAX_PORTABILTY)

#else
#define SCRIPT_REGISTER_FUNC(funcname) \
	GenericScript::funcname##_return+" "#funcname" ("+GenericScript::funcname##_arg+")", (void*)funcname, asCALL_STDCALL
#endif

namespace efe
{
	class  iScript : public iResourceBase
	{
	public:
		iScript(const tString &a_sName) : iResourceBase(a_sName, 0){}
		virtual ~iScript(){}

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}

		virtual bool CreateFromFile(const tString &a_sFileName)=0;

		virtual int GetFuncHandle(const tString &a_sFunc)=0;

		virtual void AddArg(const tString &a_sArg)=0;

		virtual bool Run(const tString &a_sFuncLine)=0;

		virtual bool Run(int a_lHandle)=0;
	};
};
#endif