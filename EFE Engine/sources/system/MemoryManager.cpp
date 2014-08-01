#include "system/MemoryManager.h"

#include "system/LowLevelSystem.h"

namespace efe
{
	tAllocatedPointerMap cMemoryManager::m_mapPointers;
	size_t cMemoryManager::m_lTotalMemoryUsage = 0;
	bool cMemoryManager::m_bLogDeletion = false;
	bool cMemoryManager::m_bLogCreation = false;
	int cMemoryManager::m_lCreationCount = 0;

	cAllocatedPointer::cAllocatedPointer(void *a_pData, const std::string &a_sFile, int a_lLine, size_t a_lMemory)
	{
		m_pData = a_pData;
		m_sFile = a_sFile;
		m_lLine = a_lLine;
		m_lMemory = a_lMemory;
	}

	//--------------------------------------------------------------

	void *cMemoryManager::AddPointer(const cAllocatedPointer &a_AllocatedPointer)
	{
		m_mapPointers.insert(tAllocatedPointerMap::value_type(a_AllocatedPointer.m_pData,a_AllocatedPointer));
		m_lTotalMemoryUsage += a_AllocatedPointer.m_lMemory;

		if (m_bLogCreation)
			m_lCreationCount++;

		return a_AllocatedPointer.m_pData;
	}

	//--------------------------------------------------------------

	bool cMemoryManager::RemovePointer(void *a_pData)
	{
		bool bFound = false;
		tAllocatedPointerMapIt it = m_mapPointers.upper_bound(a_pData);
		it--;
		if (it != m_mapPointers.end())
		{
			char *a_pTest = (char*)it->second.m_pData;
			size_t testSize = it->second.m_lMemory;
			if (a_pData >= a_pTest && a_pData < a_pTest + testSize) bFound = true;
		}

		if (bFound==false)
		{
			Warning("Trying to delete pointer %d created that does not exist!\n", a_pData);
			return false;
		}

		m_lTotalMemoryUsage -= it->second.m_lMemory;

		m_mapPointers.erase(it);

		return true;
	}

	//--------------------------------------------------------------

	void cMemoryManager::LogResults()
	{
		Log("\n|--Memory Manager Report-------------------------\n");
		Log("|\n");

		if (m_mapPointers.empty())
			Log("| No memory leaks detected. Memory left: %d\n", m_lTotalMemoryUsage);
		else
		{
			Log("| Memory leaks deteced: \n");
			Log("|\n");

			Log("| address\t file");

			int lMax = 0;
			tAllocatedPointerMapIt it = m_mapPointers.begin();
			for (; it != m_mapPointers.end(); ++it)
			{
				cAllocatedPointer &ap = it->second;
				if ((int)ap.m_sFile.length() > lMax) lMax = ap.m_sFile.length();
			}

			lMax += 5;

			for (int i=0;i<lMax-4;++i) Log(" ");

			Log("line\t\t memory left\t  \n");

			Log("|--------------------------------------------------------------\n");

			it = m_mapPointers.begin();
			for (; it != m_mapPointers.end(); ++it)
			{
				cAllocatedPointer &ap = it->second;
				Log("| %d\t %s", ap.m_pData, ap.m_sFile.c_str());
				for (int i=0; i<lMax - (int)ap.m_sFile.length(); ++i) Log(" ");
				Log("%d\t\t %d\t\n", ap.m_lLine, ap.m_lMemory);
			}
		}
		Log("|\n");
		Log("|------------------------------------------------------|\n\n");
	}

	//--------------------------------------------------------------

	void cMemoryManager::SetLogCreation(bool a_bX)
	{
		m_bLogCreation = a_bX;
	}
}