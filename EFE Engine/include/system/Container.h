#ifndef EFE_CONTAINER_H
#define EFE_CONTAINER_H

#include <vector>
#include <list>
#include <map>

#include "system/MemoryManager.h"

namespace efe
{
	class iContainerIterator
	{
		friend class cSerializeClass;
	protected:
		virtual bool HasNext()=0;

		virtual void *NextPtr()=0;
	};


	class iContainer
	{
		friend class cSerializeClass;
	public:
		virtual size_t Size()=0;
		virtual void Clear()=0;
	protected:
		virtual void AddVoidPtr(void **a_pPtr)=0;
		virtual void AddVoidClass(void *a_pClass)=0;

		virtual iContainerIterator *CreateIteratorPtr()=0;
	};

	class iContainerKeyPair
	{
	public:
		virtual size_t Size()=0;

		virtual void AddVoidPtr(void *a_pKey, void **a_pClass)=0;
		virtual void AddVoidClass(void *a_pKey, void *a_pClass)=0;
	};

	template<class T>
	class cContainerVecIterator : public iContainerIterator
	{
		void *NextPtr(){return &Next();}

	public:
		cContainerVecIterator(std::vector<T> *a_pVec)
		{
			m_pVec = a_pVec;
			m_It = a_pVec->begin();
		}

		bool HasNext()
		{
			return m_It != m_pVec->end();
		}

		T &Next()
		{
			T &val = *m_It;
			m_It++;
			return val;
		}

		T &NextPeek()
		{
			return m_It;
		}

		void Erase()
		{
			if (m_It != m_pVec->end())
				m_It = m_pVec->erase(m_It);
		}

		std::vector<T> *m_pVec;
		typename std::vector<T>::iterator m_It;
	};

	template<class T>
	class cContainerVec : public iContainer
	{
	private:
		void AddVoidPtr(void **a_pPtr)
		{
			m_vVector.push_back(*((T*)a_pPtr));
		}
		void AddVoidClass(void *a_pClass)
		{
			m_vVector.push_back(*((T*)a_pClass));
		}
		iContainerIterator *CreateIteratorPtr()
		{
			return efeNew(cContainerVecIterator<T>, (&m_vVector));
		}
	public:
		cContainerVec(){}

		size_t Size()
		{
			return m_vVector.size();
		}

		void Clear()
		{
			m_vVector.clear();
		}

		void Reserve(size_t a_lSize)
		{
			m_vVector.reserve(a_lSize);
		}
		
		void Resize(size_t a_lSize)
		{
			m_vVector.resize(m_vVector);
		}

		void Add(T a_Val)
		{
			m_vVector.push_back(a_Val);
		}
		
		cContainerVecIterator<T> GetIterator()
		{
			return cContainerVecIterator<T>(&m_vVector);
		}

		T &operator[](size_t a_lX)
		{
			return m_vVector[a_lX];
		}
	private:

		std::vector<T> m_vVector;
	};

	template<class T>
	class cContainerListIterator : public iContainerIterator
	{
		void *NextPtr(){return &Next();}
	public:
		cContainerListIterator(std::list<T> *a_pVec)
		{
			m_pVec = a_pVec;
			m_It = a_pVec->begin();
		}
		bool HasNext()
		{
			return m_It != m_pVec->end();
		}

		T &Next()
		{
			T &val = *m_It;
			m_It++;
			return val;
		}

		T &PeekNext()
		{
			return *m_It;
		}

		void Erase()
		{
			if (m_It != m_pVec->end())
				m_It = m_pVec->erase(m_It);
		}
	private:
		std::list<T> *m_pVec;
		typename std::list<T>::iterator m_It;
	};

	template<class T>
	class cContainerList : public iContainer
	{
	private:
		void AddVoidPtr(void **a_pPtr)
		{
			m_vVector.push_back(*((T*)a_pPtr));
		}
		void AddVoidClass(void *a_pClass)
		{
			m_vVector.push_back(*((T*)a_pClass));
		}
		iContainerIterator *CreateIteratorPtr()
		{
			return efeNew(cContainerListIterator<T>, (&m_vVector));
		}
	public:
		cContainerList(){}

		size_t Size()
		{
			return m_vVector.size();
		}

		void Clear()
		{
			m_vVector.clear();
		}

		void Add(T a_Val)
		{
			m_vVector.push_back(a_Val);
		}

		cContainerListIterator<T> GetIterator()
		{
			return cContainerListIterator<T>(&m_vVector);
		}
		std::list<T> m_vVector;
	};
};

#endif