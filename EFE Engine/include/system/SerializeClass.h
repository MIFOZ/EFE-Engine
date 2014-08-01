#ifndef EFE_SERIALIZE_CLASS_H
#define EFE_SERIALIZE_CLASS_H

#include <map>
#include "system/SystemTypes.h"
#include "system/MemoryManager.h"

namespace efe
{
	typedef unsigned int eSerializeType;

	#define eSerializeType_Bool				(1)
	#define eSerializeType_Int32			(2)
	#define eSerializeType_Float32			(3)

	#define eSerializeType_String			(4)

	#define eSerializeType_Vector2l			(5)
	#define eSerializeType_Vector2f			(6)
	#define eSerializeType_Vector3l			(7)
	#define eSerializeType_Vector3f			(8)

	#define eSerializeType_Matrixf			(9)

	#define eSerializeType_Color			(10)

	#define eSerializeType_Rectl			(11)
	#define eSerializeType_Rectf			(12)

	#define eSerializeType_Planef			(13)

	#define eSerializeType_WString			(14)

	#define eSerializeType_Class			(100)

	#define eSerializeType_ClassPointer		(101)

	#define eSerializeType_Last				(200)

	#define eSerializeType_NULL				(0xFFFF)

	//////////////////////////////////////////////////////////////
	// ENGINE MAIN TYPES
	//////////////////////////////////////////////////////////////

	typedef unsigned int eSerializeMainType;

	#define eSerializeMainType_Variable		(1)
	#define eSerializeMainType_Array		(2)
	#define eSerializeMainType_Container	(3)

	#define eSerializeMainType_NULL			(0xFFFF)

	//////////////////////////////////////////////////////////////
	// HELPER DEFINES
	//////////////////////////////////////////////////////////////

	#define kMaxSerializeClasses 1000

	#define ClassMemberOffset(a_Class, a_Member)	(size_t(&(((a_Class*)1)->a_Member))-1)
	#define ClassMemberSize(a_Class, a_Member)		(sizeof(((a_Class*)1)->a_Member))

	#define kSerializableClassInit(a_Class) \
			public:	\
			const static tString m_sSerialize_Name; \
			const static tString m_sSerialize_ParentName; \
			const static cSerializeMemberField *m_pSerialize_MemberFields; \
			virtual tString Serialize_GetTopClass(){return #a_Class;}

	#define kBeginSerialize(a_Class,a_Parent) \
			namespace SerializeNamespace_##a_Class \
			{ \
				extern cSerializeMemberField m_vTempMemberFields[];\
			} \
			static iSerializable *_Create_##a_Class(){return efeNew(a_Class,());} \
			static cSerializeClass g_SerializedClass_##a_Class(#a_Class, #a_Parent, SerializeNamespace_##a_Class::m_vTempMemberFields, sizeof(a_Class), _Create_##a_Class); \
			const tString a_Class::m_sSerialize_Name = #a_Class; \
			const tString a_Class::m_sSerialize_ParentName = #a_Parent; \
			const cSerializeMemberField *a_Class::m_pSerialize_MemberFields = SerializeNamespace_##a_Class::m_vTempMemberFields; \
			namespace SerializeNamespace_##a_Class \
			{\
				typedef a_Class tVarClass; \
				cSerializeMemberField m_vTempMemberFields[] = {

	#define kBeginSerializeBase(a_Class) \
			namespace SerializeNamespace_##a_Class \
			{ \
				extern cSerializeMemberField m_vTempMemberFields[];\
			} \
			static iSerializable *_Create_##a_Class(){return efeNew(a_Class,());} \
			static cSerializeClass g_SerializedClass_##a_Class(#a_Class, "", SerializeNamespace_##a_Class::m_vTempMemberFields, sizeof(a_Class), _Create_##a_Class); \
			const tString a_Class::m_sSerialize_Name = #a_Class; \
			const tString a_Class::m_sSerialize_ParentName = ""; \
			const cSerializeMemberField *a_Class::m_pSerialize_MemberFields = SerializeNamespace_##a_Class::m_vTempMemberFields; \
			namespace SerializeNamespace_##a_Class \
			{\
				typedef a_Class tVarClass; \
				cSerializeMemberField m_vTempMemberFields[] = {

	#define kBeginSerializeVirtual(a_Class, a_Parent) \
			namespace SerializeNamespace_##a_Class \
			{ \
				extern cSerializeMemberField m_vTempMemberFields[];\
			} \
			static cSerializeClass g_SerializedClass_##a_Class(#a_Class, #a_Parent, SerializeNamespace_##a_Class::m_vTempMemberFields, sizeof(a_Class), NULL); \
			const tString a_Class::m_sSerialize_Name = #a_Class; \
			const tString a_Class::m_sSerialize_ParentName = #a_Parent; \
			const cSerializeMemberField *a_Class::m_pSerialize_MemberFields = SerializeNamespace_##a_Class::m_vTempMemberFields; \
			namespace SerializeNamespace_##a_Class \
			{\
				typedef a_Class tVarClass; \
				cSerializeMemberField m_vTempMemberFields[] = {

	#define kBeginSerializeBaseVirtual(a_Class) \
			namespace SerializeNamespace_##a_Class \
			{ \
				extern cSerializeMemberField m_vTempMemberFields[];\
			} \
			static cSerializeClass g_SerializedClass_##a_Class(#a_Class, "", SerializeNamespace_##a_Class::m_vTempMemberFields, sizeof(a_Class), NULL); \
			const tString a_Class::m_sSerialize_Name = #a_Class; \
			const tString a_Class::m_sSerialize_ParentName = ""; \
			const cSerializeMemberField *a_Class::m_pSerialize_MemberFields = SerializeNamespace_##a_Class::m_vTempMemberFields; \
			namespace SerializeNamespace_##a_Class \
			{\
				typedef a_Class tVarClass; \
				cSerializeMemberField m_vTempMemberFields[] = {

	#define kEndSerialize() cSerializeMemberField("", 0,0,eSerializeType_NULL,0) \
							};}

	#define kSerializeVar(a_Var, a_Type) \
	cSerializeMemberField(#a_Var, ClassMemberOffset(tVarClass,a_Var),ClassMemberSize(tVarClass,a_Var),a_Type,eSerializeMainType_Variable),

	#define kSerializeVarArray(a_Var, a_Type, a_ArraySize) \
	cSerializeMemberField(#a_Var, ClassMemberOffset(tVarClass,a_Var),ClassMemberSize(tVarClass,a_Var),a_Type,eSerializeMainType_Array,a_ArraySize),

	#define kSerializeVarContainer(a_Var, a_Type) \
	cSerializeMemberField(#a_Var, ClassMemberOffset(tVarClass,a_Var),ClassMemberSize(tVarClass,a_Var),a_Type,eSerializeMainType_Container),
	
	#define kSerializeClassContainer(a_Var, a_Class, a_Type) \
	cSerializeMemberField(#a_Var, ClassMemberOffset(tVarClass,a_Var),ClassMemberSize(tVarClass,a_Var),a_Type,eSerializeMainType_Container,#a_Class),

	class cSerializeMemberField
	{
	public:
		cSerializeMemberField(const tString &a_sName, size_t a_lOffset, size_t a_lSize, eSerializeType a_lType,
			eSerializeMainType a_MainType)
		{
			m_sName = a_sName;
			m_lOffset = a_lOffset;
			m_lSize = a_lSize;
			m_lType = a_lType;
			m_MainType = a_MainType;
		}

		cSerializeMemberField(const tString &a_sName, size_t a_lOffset, size_t a_lSize, eSerializeType a_lType,
			eSerializeMainType a_MainType, size_t a_lArraySize)
		{
			m_sName = a_sName;
			m_lOffset = a_lOffset;
			m_lSize = a_lSize;
			m_lType = a_lType;
			m_MainType = a_MainType;
			m_lArraySize = a_lArraySize;
		}

		cSerializeMemberField(const tString &a_sName, size_t a_lOffset, size_t a_lSize, eSerializeType a_lType,
			eSerializeMainType a_MainType, const tString &a_sClassName)
		{
			m_sName = a_sName;
			m_lOffset = a_lOffset;
			m_lSize = a_lSize;
			m_lType = a_lType;
			m_MainType = a_MainType;
			m_sName = a_sClassName;
		}

		tString m_sName;
		tString m_sClassName;
		size_t m_lOffset;
		size_t m_lSize;
		eSerializeType m_lType;
		eSerializeMainType m_MainType;
		size_t m_lArraySize;
	};

	class iSerializable
	{
	public:
		virtual tString Serialize_GetTopClass()
		{
			return "";
		}
	};

	class iSerializableType
	{
	public:
		virtual char *ValueToString(void *a_pVal)=0;
		virtual void ValueFromString(char *a_pString, void *a_pVal)=0;
	};

	typedef struct cSerializeSavedClass
	{
	public:
		cSerializeSavedClass(){}
		cSerializeSavedClass(const char *a_sName, const char *a_sParent,
							cSerializeMemberField *a_pMemberFields, size_t a_lSize,
							iSerializable *(*a_pCreateFunc)());

		const char *m_sName;
		const char *m_sParentName;
		cSerializeMemberField *m_pMemberFields;
		size_t m_lSize;
		iSerializable *(*m_pCreateFunc());
	} cSerializeSavedClass;

	class cSerializeMemberFieldIterator
	{
	public:
		cSerializeMemberFieldIterator(cSerializeSavedClass *a_pTopClass);

		bool *HasNext();
		cSerializeMemberField *GetNext();
	private:
		cSerializeSavedClass *m_pSavedClass;
		int m_lFieldNum;
	};

	//--------------------------------------------------------------

	typedef std::map<tString, cSerializeSavedClass> tSerializeSavedClassMap;
	typedef tSerializeSavedClassMap::iterator tSerializeSavedClassMapIt;

	typedef std::list<cSerializeSavedClass*> tSerializeSavedClassList;
	typedef tSerializeSavedClassList::iterator tSerializeSavedClassListIt;

	class cSerializeClass
	{
	public:
		cSerializeClass(const char *a_sName, const char *a_sParent, cSerializeMemberField *a_pMemberFields,
							size_t a_lSize, iSerializable *(*a_pCreateFunc)());
	private:
		//static cSerializeMemberField &GetMemberField(const tString a_sName, cSerializeSavedClass *a_pClass);

		static void SetUpData();

		static tString m_sTempString;
		static char m_sTempCharArray[2048];

		static bool m_bDataSetup;

	};
};
#endif