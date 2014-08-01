#ifndef EFE_MEMORY_MANAGER_H
#define EFE_MEMORY_MANAGER_H

#include <map>
#include <string>

namespace efe
{
	class cAllocatedPointer
	{
	public:
		cAllocatedPointer(void *a_pData, const std::string &a_sFile, int a_lLine, size_t a_lMemory);

		std::string m_sFile;
		int m_lLine;
		size_t m_lMemory;
		void *m_pData;
	};

	typedef std::map<void *, cAllocatedPointer> tAllocatedPointerMap;
	typedef tAllocatedPointerMap::iterator tAllocatedPointerMapIt;

	class cMemoryManager
	{
	public:
		static void *AddPointer(const cAllocatedPointer &a_AllocatedPointer);

		static bool RemovePointer(void *a_pData);

		static void LogResults();

		static tAllocatedPointerMap m_mapPointers;
		static size_t m_lTotalMemoryUsage;

		static bool m_bLogDeletion;

		template<class T>
		static T *DeleteAndReturn(T *a_pData)
		{
			delete a_pData;
			return a_pData;
		}

		template<class T>
		static T *DeleteArrayAndReturn(T *a_pData)
		{
			delete [] a_pData;
			return a_pData;
		}

		template<class T>
		static T *FreeAndReturn(T *a_pData)
		{
			free(a_pData);
			return a_pData;
		}

		void SetLogCreation(bool a_bX);
		static bool GetLogCreation(){return m_bLogCreation;}

		static int GetCreationCount(){return m_lCreationCount;}
	private:
		static bool m_bLogCreation;
		static int m_lCreationCount;
	};

#define MEMORY_MANAGER_ACTIVE
#ifdef MEMORY_MANAGER_ACTIVE
#define efeNew(classType, constructor) \
	(classType *)efe::cMemoryManager::AddPointer((efe::cAllocatedPointer(new classType constructor,__FILE__,__LINE__,sizeof(classType))))

#define efeNewArray(classType, amount) \
	(classType *)efe::cMemoryManager::AddPointer((efe::cAllocatedPointer(new classType[amount],__FILE__,__LINE__,amount * sizeof(classType))))

#define efeMalloc(amount) \
	efe::cMemoryManager::AddPointer(efe::cAllocatedPointer(malloc(amount),__FILE__,__LINE__,amount))

#define efeDelete(data){ \
	if (efe::cMemoryManager::RemovePointer(efe::cMemoryManager::DeleteAndReturn(data)) == false) efe::Log("Deleting at '%s' %d\n",__FILE__,__LINE__); \
	}

#define efeDeleteArray(data){ \
	if (efe::cMemoryManager::RemovePointer(efe::cMemoryManager::DeleteArrayAndReturn(data)) == false) efe::Log("Deleting at '%s' %d\n",__FILE__,__LINE__); \
	}

#define efeFree(data){ \
	if (efe::cMemoryManager::RemovePointer(efe::cMemoryManager::FreeAndReturn(data)) == false) efe::Log("Deleting at '%s' %d\n",__FILE__,__LINE__); \
	}
#else
#define efeNew(classType, constructor) \
	new classType constructor

#define efeNewArray(classType, amount) \
	new classType[amount]

#define efeMalloc(amount) malloc(amount)
	

#define efeDelete(data) \
	delete data;

#define efeDeleteArray(data) \
	delete [] data;

#define efeFree(data) \
	free(data);
#endif
};

#endif