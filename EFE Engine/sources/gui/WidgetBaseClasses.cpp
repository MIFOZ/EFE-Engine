#include "gui/WidgetBaseClasses.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Widget.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// ITEM CONTAINER
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iWidgetItemContainer::AddItem(const tWString &a_sItem)
	{
		m_vItems.push_back(a_sItem);

		UpdateProperties();
	}

	void iWidgetItemContainer::RemoveItem(int a_lX)
	{
		int lCount = 0;
		tWStringVecIt it = m_vItems.begin();
		for (; it != m_vItems.end(); ++it, ++lCount)
		{
			if (a_lX == lCount)
			{
				m_vItems.erase(it);
				break;
			}
		}

		UpdateProperties();
	}

	void iWidgetItemContainer::RemoveItem(const tWString &a_sItem)
	{
		tWStringVecIt it = m_vItems.begin();
		for (; it != m_vItems.end(); ++it)
		{
			if (*it == a_sItem)
			{
				m_vItems.erase(it);
				break;
			}
		}

		UpdateProperties();
	}

	//--------------------------------------------------------------

	const tWString &iWidgetItemContainer::GetItem(int a_lX)
	{
		return m_vItems[a_lX];
	}
	void iWidgetItemContainer::SetItem(int a_lX, const tWString &a_sItem)
	{
		m_vItems[a_lX] = a_sItem;
	}

	int iWidgetItemContainer::GetItemNum() const
	{
		return (int)m_vItems.size();
	}
	bool iWidgetItemContainer::HasItem(const tWString &a_sItem)
	{
		tWStringVecIt it = m_vItems.begin();
		for (; it != m_vItems.end(); ++it)
		{
			if (*it == a_sItem) return true;
		}

		return false;
	}
}