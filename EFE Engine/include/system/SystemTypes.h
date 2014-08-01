#ifndef EFE_SYSTEM_TYPES_H
#define EFE_SYSTEM_TYPES_H

#include <stdio.h>
#include <string>
#include <list>
#include <set>
#include <vector>

#include <assert.h>

#include "system/Container.h"

namespace efe
{
	#define eFlagBit_All		(0xFFFFFFFF)

	#define eFlagBit_0		(0x00000001)
	#define eFlagBit_1		(0x00000002)
	#define eFlagBit_2		(0x00000004)
	#define eFlagBit_3		(0x00000008)

	#define _W(t) L ## t

	enum eSystemPath
	{
		eSystemPath_Personal,
		eSystemPath_LastEnum
	};

	typedef unsigned int tFlag;

	typedef std::string tString;

	typedef std::list<tString> tStringList;
	typedef tStringList::iterator tStringListIt;

	typedef std::set<tString> tStringSet;
	typedef tStringSet::iterator tStringSetIt;

	typedef std::vector<tString> tStringVec;
	typedef tStringVec::iterator tStringVecIt;


	typedef std::wstring tWString;
	typedef std::list<tWString> tWStringList;
	typedef tWStringList::iterator tWStringListIt;

	typedef std::vector<tWString> tWStringVec;
	typedef tWStringVec::iterator tWStringVecIt;

	typedef std::vector<unsigned int> tUIntVec;
	typedef tUIntVec::iterator tUIntVecIt;

	typedef std::vector<int> tIntVec;
	typedef tIntVec::iterator tIntVecIt;

	typedef std::vector<float> tFloatVec;
	typedef tFloatVec::iterator tFloatVecIt;

	typedef std::list<unsigned int> tUIntList;
	typedef tUIntList::iterator tUIntListIt;

	typedef enum
	{
		eMsgBoxType_Info,
		eMsgBoxType_Error,
		eMsgBoxType_Warning,
		eMsgBoxType_Default
	} eMsgBoxType;

#define STLCallInAll(ContType, a_Cont, a_Func)					\
	{															\
		##ContType##::iterator it = ##a_Cont##.begin();			\
		for (;it != ##a_Cont##.end(); it++) (*it)->##a_Func##;	\
	}

	class cDate
	{
	public:
		int seconds;
		int minuts;
		int hours;
		int month_day;
		int month;
		int year;
		int week_day;
		int year_day;

		tString ToString()
		{
			char buff[256];

			sprintf_s(buff, "%d/%d/%d %d:%d:%d", month_day, month,1900+year, hours, minuts, seconds);

			return buff;
		}

		bool operator==(const cDate &a_Date) const
		{
			if (seconds == a_Date.seconds &&
				minuts == a_Date.minuts &&
				hours == a_Date.hours &&
				month_day == a_Date.month_day &&
				month == a_Date.month &&
				year == a_Date.year)
			{
				return true;
			}

			return false;
		}

		bool operator!=(const cDate &a_Date) const
		{
			return !(*this==a_Date);
		}

		bool operator>(const cDate &a_Date) const
		{
			if (year > a_Date.year) return true;
			else if (year < a_Date.year) return false;

			if (month > a_Date.month) return true;
			else if (month < a_Date.month) return false;

			if (month_day > a_Date.month_day) return true;
			else if (month_day < a_Date.month_day) return false;

			if (hours > a_Date.hours) return true;
			else if (hours < a_Date.hours) return false;

			if (minuts > a_Date.minuts) return true;
			else if (minuts < a_Date.minuts) return false;

			if (seconds > a_Date.seconds) return true;
			else if (seconds < a_Date.seconds) return false;

			return false;
		}
	};

	//--------------------------------------------------------------

	template <class T> class cMemoryPool
	{
	public:
		cMemoryPool(size_t a_lSize, T* (*a_pCreateFunc)())
		{
			m_vData.resize(a_lSize,NULL);
			m_lCurrentData = 0;

			m_pCreateFunc = a_pCreateFunc;

			for (size_t i=0;i<m_vData.size();++i)
			{
				if (m_pCreateFunc)	m_vData[i] = m_pCreateFunc();
				else				m_vData[i] = efeNew(T, ());
			}
		}

		~cMemoryPool()
		{
			for (size_t i=0;i<m_vData.size();++i) efeDelete(m_vData[i]);
		}

		T *Create()
		{
			T *pData = m_vData[m_lCurrentData];

			if (m_lCurrentData == m_vData.size()-1)
			{
				size_t lNewSize = m_vData.size() * 2;
				size_t lStart = m_vData.size();
				m_vData.resize(lNewSize);

				for (size_t i=lStart;i<m_vData.size();++i)
				{
					if (m_pCreateFunc)	m_vData[i] = m_pCreateFunc();
					else				m_vData[i] = efeNew(T, ());
				}
			}

			m_lCurrentData++;

			return pData;
		}

		void Release(T *a_pData)
		{
			if (m_lCurrentData==0)
				return;

			--m_lCurrentData;
			m_vData[m_lCurrentData] = a_pData;
		}

		void ClearUnused()
		{
			for (size_t i=m_lCurrentData;i<m_vData.size();++i)
				efeDelete(m_vData[i]);

			m_vData.resize(m_lCurrentData+1);
		}

	private:
		std::vector<T*> m_vData;

		size_t m_lCurrentData;

		T *(*m_pCreateFunc)();
	};

	//--------------------------------------------------------------

	template <class CONT, class T>
	void STLFindAndDelete(CONT &a_Cont, T *pObject)
	{
		typename CONT::iterator it = a_Cont.begin();
		for(;it != a_Cont.end(); it++)
		{
			if (*it == pObject)
			{
				a_Cont.erase(it);
				break;
			}
		}
		efeDelete(pObject);
	};

	//--------------------------------------------------------------

	template <class CONT>
	void *STLFindByName(CONT &a_Cont, const tString &a_sName)
	{
		typename CONT::iterator it = a_Cont.begin();
		for(;it != a_Cont.end(); it++)
		{
			if ((*it)->GetName() == a_sName)
			{
				return *it;
			}
		}
		return NULL;
	};

	//--------------------------------------------------------------

	template <class T>
	void STLDeleteAll(T &a_Cont)
	{
		typename T::iterator it = a_Cont.begin();
		for (;it != a_Cont.end();it++)
			efeDelete(*it);
		a_Cont.clear();
	}

	template <class T>
	void STLMapDeleteAll(T &a_Cont)
	{
		typename T::iterator it = a_Cont.begin();
		for (;it != a_Cont.end();it++)
			efeDelete(it->second);
		a_Cont.clear();
	}

	template <class T, class CONT, class IT>
	class cSTLIterator : public iContainerIterator
	{
	public:
		cSTLIterator(CONT *a_pCont)
		{
			m_pCont = a_pCont;
			m_It = m_pCont->begin();
		}

		bool HasNext()
		{
			return m_It != m_pCont->end();
		}

		void *NextPtr()
		{
			if (m_It == m_pCont->end())
				return NULL;
			else
			{
				T &temp = const_cast<T &>(*m_It);
				m_It++;
				return &temp;
			}
		}

		T Next()
		{
			if (m_It == m_pCont->end())
				return NULL;
			else
			{
				T &temp = const_cast<T &>(*m_It);
				m_It++;
				return temp;
			}
		}

		T PeekNext()
		{
			if (m_It == m_Cont->end())
				return NULL;
			else return *m_It;
		}
	private:
		IT m_It;
		CONT *m_pCont;
	};

	template <class T, class CONT, class IT>
	class cSTLMapIterator : public iContainerIterator
	{
	public:
		cSTLMapIterator(CONT *a_pCont)
		{
			m_pCont = a_pCont;
			m_It = m_pCont->begin();
		}

		bool HasNext()
		{
			return m_It != m_pCont->end();
		}

		void *NextPtr()
		{
			if (m_It == m_pCont->end())
				return NULL;
			else
			{
				T &temp = m_It->second;
				m_It++;
				return &temp;
			}
		}

		T Next()
		{
			if (m_It == m_pCont->end())
				return NULL;
			else
			{
				T &temp = m_It->second;
				m_It++;
				return temp;
			}
		}

		T PeekNext()
		{
			if (m_It == m_Cont->end())
				return NULL;
			else return m_It->second;
		}
	private:
		IT m_It;
		CONT *m_pCont;
	};
};
#endif