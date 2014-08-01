#ifndef EFE_BINTREE_H
#define  EFE_BINTREE_H

#include <list>

namespace efe
{
	enum eBinTreeNode
	{
		eBinTreeNode_Left,
		eBinTreeNode_Right
	};

	template <class T> class BinTreeNode
	{
	public:
		T *GetData(){return &m_Data;}

		BinTreeNode(T a_Data, BinTreeNode<T> *a_Parent, eBinTreeNode a_ParentDir)
		{
			for (int i=0;i<2;i++) m_Child[i]=NULL;
			m_Data = a_Data;
			m_Parent = a_Parent;
			m_ParentDir = a_ParentDir;
		}

		BinTreeNode<T> *AddChild(eBinTreeNode i, T a_Data)
		{
			if (m_Child[i]==NULL)
			{
				m_Child[i] = efeNew(BinTreeNode<T>, (a_Data,this, i));
				return m_Child[i];
			}
			return NULL;
		}

		BinTreeNode<T> *GetChild(eBinTreeNode i){return m_Child[i];}

		BinTreeNode<T> *GetParent(){return m_pParent;}
	private:
		BinTreeNode<T> *m_Child[2];
		BinTreeNode<T> *m_Parent;
		T m_Data;
		eBinTreeNode m_ParentDir;
	};

	template<class T> class BinTree
	{
	public:
		BinTree()
		{
			m_lNumOfNodes = 0;
			m_FirstNode = NULL;
		}

		~BinTree()
		{
			Clear();
		}

		int Clear()
		{
			m_lNum = 0;
			DeleteNode(m_FirstNode);
			m_FirstNode = NULL;
			return m_lNum;
		}

		BinTreeNode<T> *Insert(T a_Data)
		{
			if (m_FirstNode==NULL)
			{
				m_FirstNode = efeNew(BinTreeNode<T>, (a_Data, NULL, eBinTreeNode_Left));
				m_lNumOfNodes++;

				return m_FirstNode;
			}
			BinTreeNode<T> *Node = m_FirstNode;
			eBinTreeNode c;
			while(true)
			{
				c = eBinTreeNode_Left;

				if (Node->GetChild(c)==NULL)
				{
					Node = Node->AddChild(c, a_Data);
					break;
				}
				else
					Node = Node->GetChild(c);
			}
			m_lNumOfNodes++;

			return Node;
		}

		BinTreeNode<T> *InsertAt(T a_Data, BinTreeNode<T> *a_Node, eBinTreeNode a_Child=eBinTreeNode_Left)
		{
			if (a_Node==NULL) return NULL;

			if (a_Node->GetChild(a_Child)!=NULL)
			{
				a_Child = a_Child==eBinTreeNode_Left ? eBinTreeNode_Right : eBinTreeNode_Left;
				if (a_Node->GetChild(a_Child)!=NULL) return NULL;
			}

			return a_Node->AddChild(a_Child, a_Data);
		}

		int Size()
		{
			return m_lNumOfNodes;
		}

		const std::list<BinTreeNode<T>*> &GetLeafList()
		{
			m_lstNodes.clear();
			PopulateLeafList(m_FirstNode);
			return m_lstNodes;
		}

		const std::list<BinTreeNode<T>*> &GetNodeList()
		{
			m_lstNodes.clear();
			PopulateNodeList(m_FirstNode);
			return m_lstNodes;
		}
	private:
		int m_lNumOfNodes;
		BinTreeNode<T> *m_FirstNode;
		int m_lNum;

		std::list<BinTreeNode<T>*> m_lstNodes;

		void DeleteNode(BinTreeNode<T> *a_Node)
		{
			if (a_Node==NULL) return;

			DeleteNode(a_Node->GetChild(eBinTreeNode_Left));
			DeleteNode(a_Node->GetChild(eBinTreeNode_Right));

			efeDelete(a_Node);
			m_lNum++;
		}

		void PopulateNodeList(BinTreeNode<T> *a_Node)
		{
			if (a_Node==NULL) return;

			PopulateNodeList(a_Node->GetChild(eBinTreeNode_Left));
			m_lstNodes.push_back(a_Node);
			PopulateNodeList(a_Node->GetChild(eBinTreeNode_Right));
		}

		void PopulateLeafList(BinTreeNode<T> *a_Node)
		{
			if (a_Node==NULL) return;

			if (a_Node->GetChild(eBinTreeNode_Left)==NULL &&
				a_Node->GetChild(eBinTreeNode_Right)==NULL)
				m_lstNodes.push_back(a_Node);

			PopulateLeafList(a_Node->GetChild(eBinTreeNode_Left));
			PopulateLeafList(a_Node->GetChild(eBinTreeNode_Right));
		}
	};
};
#endif